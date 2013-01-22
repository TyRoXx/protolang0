#include "interpreter.hpp"
#include "table.hpp"
#include <cassert>
#include <functional>
#include <boost/static_assert.hpp>


namespace p0
{
	namespace run
	{
		struct local_frame
		{
			explicit local_frame(const std::vector<value> &arguments)
				: m_values(arguments)
			{
			}

			value &get(std::size_t address)
			{
				if (address >= m_values.size())
				{
					m_values.resize(address + 1);
				}
				return m_values[address];
			}

		private:

			std::vector<value> m_values;
		};

		namespace
		{
			unsigned flag(unsigned index)
			{
				return (1u << index);
			}
		}


		interpreter::interpreter(intermediate::unit const &program)
			: m_program(program)
		{
		}

		value interpreter::call(intermediate::function const &function,
							   const std::vector<value> &arguments)
		{
			local_frame locals(arguments);

			auto const &code = function.body();
			auto current_instr = code.begin();
			while (current_instr != code.end())
			{
				auto const operation = current_instr->type();
				auto const &instr_arguments = current_instr->arguments();

				using namespace intermediate::instruction_type;

				BOOST_STATIC_ASSERT(intermediate::instruction_type::count_ == 33);

				switch (operation)
				{
				case nothing:
					break;

				case return_:
				{
					auto const result_address = static_cast<size_t>(instr_arguments[0]);
					return locals.get(result_address);
				}

				case set_from_constant:
				{
					auto const dest_address = static_cast<size_t>(instr_arguments[0]);
					auto const constant = instr_arguments[1];
					locals.get(dest_address) = value(static_cast<integer>(constant));
					break;
				}

				case set_null:
				{
					auto const dest_address = static_cast<size_t>(instr_arguments[0]);
					locals.get(dest_address) = value();
					break;
				}

				case set_function:
				{
					auto const dest_address = static_cast<size_t>(instr_arguments[0]);
					auto const function_index = static_cast<size_t>(instr_arguments[1]);
					if (function_index >= m_program.functions().size())
					{
						throw std::runtime_error("Invalid function id");
					}
					auto const &function_ptr = m_program.functions()[function_index];
					locals.get(dest_address) = value(function_ptr);
					break;
				}

				case set_string:
					break;

				case copy:
				{
					auto const dest_address = static_cast<size_t>(instr_arguments[0]);
					auto const source_address = static_cast<size_t>(instr_arguments[1]);
					locals.get(dest_address) = locals.get(source_address);
					break;
				}

				case add:
				case sub:
				case mul:
				case div:
				case mod:
				case and_:
				case or_:
				case xor_:
				case shift_left:
				case shift_right:
				{
					auto const dest_address = static_cast<size_t>(instr_arguments[0]);
					auto const source_address = static_cast<size_t>(instr_arguments[1]);
					auto &dest = locals.get(dest_address);
					auto const &source = locals.get(source_address);
					if ((dest.type == value_type::integer) &&
						(source.type == value_type::integer))
					{
						static auto const check_divisor = [](integer divisor)
						{
							if (divisor == 0)
							{
								throw std::runtime_error("Division by zero");
							}
						};
						switch (operation)
						{
						case add: dest.i += source.i; break;
						case sub: dest.i -= source.i; break;
						case mul: dest.i *= source.i; break;
						case div: check_divisor(source.i); dest.i /= source.i; break;
						case mod: check_divisor(source.i); dest.i %= source.i; break;
						case and_: dest.i &= source.i; break;
						case or_: dest.i |= source.i; break;
						case xor_: dest.i ^= source.i; break;
						case shift_left: dest.i <<= source.i; break;
						case shift_right: dest.i >>= source.i; break;
						default: assert(!"missing operation"); break;
						}
					}
					break;
				}

				case not_:
				{
					auto const operand_address = static_cast<size_t>(instr_arguments[0]);
					auto &operand = locals.get(operand_address);
					operand = value(static_cast<integer>(to_boolean(operand)));
					break;
				}

				case invert:
				{
					auto const operand_address = static_cast<size_t>(instr_arguments[0]);
					auto &operand = locals.get(operand_address);
					if (operand.type == value_type::integer)
					{
						operand.i = ~operand.i;
					}
					break;
				}

				case negate:
				{
					auto const operand_address = static_cast<size_t>(instr_arguments[0]);
					auto &operand = locals.get(operand_address);
					if (operand.type == value_type::integer)
					{
						operand.i = -operand.i;
					}
					break;
				}

				case equal:
				case not_equal:
				case less:
				case less_equal:
				case greater:
				case greater_equal:
				{
					auto const left_address = static_cast<size_t>(instr_arguments[0]);
					auto const right_address = static_cast<size_t>(instr_arguments[1]);
					auto &left = locals.get(left_address);
					auto const &right = locals.get(right_address);
					int const status = compare(left, right);
					namespace co = comparison_result;
					static std::array<unsigned, 6> const expected_status =
					{{
						flag(co::equal),
						flag(co::less) | flag(co::greater),
						flag(co::less),
						flag(co::less) | flag(co::equal),
						flag(co::greater),
						flag(co::greater) | flag(co::equal),
					}};
					bool const result = ((flag(status) & expected_status[operation - equal]) != 0);
					left = value(static_cast<integer>(result ? 1 : 0));
					break;
				}

				case intermediate::instruction_type::call:
				{
					auto const function_address = static_cast<size_t>(instr_arguments[0]);
					auto const argument_count = static_cast<size_t>(instr_arguments[1]);
					auto const arguments_address = function_address + 1;
					std::vector<value> arguments;
					for (size_t i = 0; i < argument_count; ++i)
					{
						arguments.push_back(locals.get(arguments_address + i));
					}
					auto &result = locals.get(function_address);
					auto const &function = locals.get(function_address);
					switch (function.type)
					{
					case value_type::function_ptr:
						result = this->call(*function.function_ptr, arguments);
						break;

					default:
						throw std::runtime_error("Cannot call non-function value");
					}
					break;
				}

				case jump:
				{
					auto const destination = static_cast<size_t>(instr_arguments[0]);
					if (destination > code.size())
					{
						throw std::runtime_error("Invalid jump destination");
					}
					current_instr = (code.begin() + destination);
					--current_instr;
					break;
				}

				case jump_if:
				case jump_if_not:
				{
					auto const destination = static_cast<size_t>(instr_arguments[0]);
					if (destination > code.size())
					{
						throw std::runtime_error("Invalid jump destination");
					}
					auto const condition_address = static_cast<size_t>(instr_arguments[1]);
					auto const &condition = locals.get(condition_address);
					if (to_boolean(condition) == (operation == jump_if))
					{
						current_instr = (code.begin() + destination);
						--current_instr;
					}
					break;
				}

				case new_table:
				{
					auto const table_address = static_cast<size_t>(instr_arguments[1]);
					auto &destination = locals.get(table_address);
					collect_garbage(); //TODO: do this less often
					std::unique_ptr<object> created_table(new table);
					value const created_table_ptr(static_cast<table &>(*created_table));
					m_gc.add_object(std::move(created_table));
					destination = created_table_ptr;
					break;
				}

				case set_element:
					break;

				case get_element:
					break;

				default:
					assert(!"invalid operation id");
					break;
				}

				++current_instr;
			}

			return value();
		}


		void interpreter::collect_garbage()
		{
			m_gc.mark();
			//TODO: mark locals
			//m_gc.sweep();
		}
	}
}

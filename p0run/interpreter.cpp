#include "interpreter.hpp"
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

				BOOST_STATIC_ASSERT(intermediate::instruction_type::count_ == 35);

				switch (operation)
				{
				case nothing:
					break;

				case return_:
				{
					auto const result_address = static_cast<size_t>(instr_arguments[0]);
					return locals.get(result_address);
				}

				case allocate:
					break;

				case deallocate:
					break;

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
						static std::array<std::function<void ()>, 5> const cases =
						{{
							[&](){ dest.i += source.i; },
							[&](){ dest.i -= source.i; },
							[&](){ dest.i *= source.i; },
							[&](){ check_divisor(source.i); dest.i /= source.i; },
							[&](){ check_divisor(source.i); dest.i %= source.i; },
						}};
						cases[operation - add]();
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

				case and_:
					break;

				case or_:
					break;

				case xor_:
					break;

				case shift_left:
					break;

				case shift_right:
					break;

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
					break;

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
					break;

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
	}
}

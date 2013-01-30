#include "interpreter.hpp"
#include "table.hpp"
#include "string.hpp"
#include <cassert>
#include <functional>
#include <boost/static_assert.hpp>


namespace p0
{
	namespace run
	{
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

		value interpreter::call(
			intermediate::function const &function,
			const std::vector<value> &arguments)
		{
			m_locals.clear();
			m_locals.push_back(value(function));
			m_locals.insert(m_locals.end(), arguments.begin(), arguments.end());
			native_call(0, arguments.size());
			assert(!m_locals.empty());
			return m_locals.front();
		}

		void interpreter::collect_garbage()
		{
			m_gc.mark();
			std::for_each(m_locals.begin(), m_locals.end(),
				[](value const &variable)
			{
				if (variable.type == value_type::object)
				{
					variable.obj->mark();
				}
			});
			m_gc.sweep();
		}


		void interpreter::native_call(
			std::size_t arguments_address,
			std::size_t argument_count)
		{
			size_t const local_frame = arguments_address;
			
			auto const function_var = get(local_frame, 0);
			if (function_var.type != value_type::function_ptr)
			{
				throw std::runtime_error("Cannot call non-function-ptr value");
			}
			auto const &function = *function_var.function_ptr;
			auto const &code = function.body();
			auto current_instr = code.begin();
			while (current_instr != code.end())
			{
				auto const operation = current_instr->type();
				auto const &instr_arguments = current_instr->arguments();

				using namespace intermediate::instruction_type;

				BOOST_STATIC_ASSERT(intermediate::instruction_type::count_ == 32);

				switch (operation)
				{
				case nothing:
					{
						break;
					}

				case set_from_constant:
					{
						auto const dest_address = static_cast<size_t>(instr_arguments[0]);
						auto const constant = instr_arguments[1];
						get(local_frame, dest_address) = value(static_cast<integer>(constant));
						break;
					}

				case set_null:
					{
						auto const dest_address = static_cast<size_t>(instr_arguments[0]);
						get(local_frame, dest_address) = value();
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
						get(local_frame, dest_address) = value(function_ptr);
						break;
					}

				case set_string:
					{
						auto const dest_address = static_cast<size_t>(instr_arguments[0]);
						auto const string_id = static_cast<size_t>(instr_arguments[1]);
						if (string_id >= m_program.strings().size())
						{
							throw std::runtime_error("Invalid string id");
						}
						auto const &content = m_program.strings()[string_id];
						std::unique_ptr<object> new_string(new string(content));
						value const new_string_ptr(*new_string);
						m_gc.add_object(std::move(new_string));
						get(local_frame, dest_address) = new_string_ptr;
						break;
					}

				case copy:
					{
						auto const dest_address = static_cast<size_t>(instr_arguments[0]);
						auto const source_address = static_cast<size_t>(instr_arguments[1]);
						auto const value = get(local_frame, source_address);
						//make copy because get(..) may reallocate
						get(local_frame, dest_address) = value;
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
						auto const source = get(local_frame, source_address);
						auto &dest = get(local_frame, dest_address);
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
						auto &operand = get(local_frame, operand_address);
						operand = value(static_cast<integer>(to_boolean(operand)));
						break;
					}

				case invert:
					{
						auto const operand_address = static_cast<size_t>(instr_arguments[0]);
						auto &operand = get(local_frame, operand_address);
						if (operand.type == value_type::integer)
						{
							operand.i = ~operand.i;
						}
						break;
					}

				case negate:
					{
						auto const operand_address = static_cast<size_t>(instr_arguments[0]);
						auto &operand = get(local_frame, operand_address);
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
						auto const right = get(local_frame, right_address);
						auto &left = get(local_frame, left_address);
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
						auto const arguments_address = static_cast<size_t>(instr_arguments[0]);
						auto const argument_count = static_cast<size_t>(instr_arguments[1]);
						native_call(local_frame + arguments_address, argument_count);
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
						auto const &condition = get(local_frame, condition_address);
						if (to_boolean(condition) == (operation == jump_if))
						{
							current_instr = (code.begin() + destination);
							--current_instr;
						}
						break;
					}

				case new_table:
					{
						auto const table_address = static_cast<size_t>(instr_arguments[0]);
						auto &destination = get(local_frame, table_address);
						std::unique_ptr<object> created_table(new table);
						value const created_table_ptr(*created_table);
						m_gc.add_object(std::move(created_table));
						destination = created_table_ptr;
						break;
					}

				case set_element:
					{
						auto const table_address = static_cast<size_t>(instr_arguments[0]);
						auto const key_address = static_cast<size_t>(instr_arguments[1]);
						auto const value_address = static_cast<size_t>(instr_arguments[2]);
						auto const table = get(local_frame, table_address);
						auto const key = get(local_frame, key_address);
						auto const value = get(local_frame, value_address);
						if (table.type != value_type::object)
						{
							throw std::runtime_error("Cannot set element of non-object");
						}
						if (!table.obj->set_element(key, value))
						{
							throw std::runtime_error("Cannot set element of this object");
						}
						break;
					}

				case get_element:
					{
						auto const table_address = static_cast<size_t>(instr_arguments[0]);
						auto const key_address = static_cast<size_t>(instr_arguments[1]);
						auto const value_address = static_cast<size_t>(instr_arguments[2]);
						auto const table = get(local_frame, table_address);
						auto const key = get(local_frame, key_address);
						auto const value = get(local_frame, value_address);
						if (table.type != value_type::object)
						{
							throw std::runtime_error("Cannot get element of non-object");
						}
						auto const result = table.obj->get_element(key);
						if (!result)
						{
							throw std::runtime_error("Cannot get element of this object");
						}
						get(local_frame, value_address) = *result;
						break;
					}

				default:
					assert(!"invalid operation id");
					break;
				}

				++current_instr;
			}

			//pop everything but the return value from the stack
			m_locals.resize(local_frame + 1);
		}

		value &interpreter::get(std::size_t local_frame, std::size_t address)
		{
			auto const absolute_address = local_frame + address;
			if (absolute_address >= m_locals.size())
			{
				m_locals.resize(absolute_address + 1);
			}
			return m_locals[absolute_address];
		}
	}
}

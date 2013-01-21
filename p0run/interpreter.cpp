#include "interpreter.hpp"
#include <cassert>
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
				{
					auto const dest_address = static_cast<size_t>(instr_arguments[0]);
					auto const source_address = static_cast<size_t>(instr_arguments[1]);
					auto &dest = locals.get(dest_address);
					auto &source = locals.get(source_address);
					if ((dest.type == value_type::integer) &&
						(source.type == value_type::integer))
					{
						dest.i += source.i;
					}
					break;
				}

				case sub:
					break;

				case mul:
					break;

				case div:
					break;

				case mod:
					break;

				case not_:
					break;

				case invert:
					break;

				case negate:
					break;

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
					break;

				case not_equal:
					break;

				case less:
					break;

				case less_equal:
					break;

				case greater:
					break;

				case greater_equal:
					break;

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

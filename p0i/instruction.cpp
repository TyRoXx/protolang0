#include "instruction.hpp"


namespace p0
{
	namespace intermediate
	{
		instruction_info const &get_instruction_info(instruction_type::Enum instruction)
		{
			static std::array<instruction_info, instruction_type::count_> const infos =
			{{
				{"nothing", 0},
				{"return", 0},
				{"allocate", 1},
				{"deallocate", 1},
				{"set_from_constant", 2},
				{"set_null", 1},
				{"set_function", 2},
				{"set_string", 2},
				{"copy", 2},
				{"add", 2},
				{"sub", 2},
				{"mul", 2},
				{"div", 2},
				{"mod", 2},
				{"not", 1},
				{"invert", 1},
				{"negate", 1},
				{"and", 2},
				{"or", 2},
				{"xor", 2},
				{"shift_left", 2},
				{"shift_right", 2},
				{"equal", 2},
				{"not_equal", 2},
				{"less", 2},
				{"less_equal", 2},
				{"greater", 2},
				{"greater_equal", 2},
				{"call", 1},
				{"jump", 1},
				{"jump_if", 2},
				{"new_table", 1},
				{"set_element", 3},
				{"get_element", 3},
			}};

			return infos[instruction];
		}

		bool is_binary_arithmetic(instruction_type::Enum instruction)
		{
			return
				(instruction >= instruction_type::add) &&
				(instruction <= instruction_type::greater_equal);
		}


		instruction::instruction()
			: m_type(instruction_type::nothing)
		{
		}

		instruction::instruction(
			instruction_type::Enum type,
			argument_array const &arguments
			)
			: m_type(type)
			, m_arguments(arguments)
		{
		}

		instruction::instruction(
			instruction_type::Enum type,
			instruction_argument argument0
			)
			: m_type(type)
		{
			m_arguments[0] = argument0;
		}

		instruction::instruction(
			instruction_type::Enum type,
			instruction_argument argument0,
			instruction_argument argument1
			)
			: m_type(type)
		{
			m_arguments[0] = argument0;
			m_arguments[1] = argument1;
		}

		instruction::instruction(
			instruction_type::Enum type,
			instruction_argument argument0,
			instruction_argument argument1,
			instruction_argument argument2
			)
			: m_type(type)
		{
			m_arguments[0] = argument0;
			m_arguments[1] = argument1;
			m_arguments[2] = argument2;
		}

		instruction_type::Enum instruction::type() const
		{
			return m_type;
		}

		instruction::argument_array const &instruction::arguments() const
		{
			return m_arguments;
		}

		instruction::argument_array &instruction::arguments()
		{
			return m_arguments;
		}
	}
}

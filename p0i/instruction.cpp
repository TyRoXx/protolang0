#include "instruction.hpp"
#include <string>
#include <boost/tuple/tuple_comparison.hpp>
#include <boost/tuple/tuple.hpp>


namespace p0
{
	namespace intermediate
	{
		std::vector<instruction_argument_type> make_binary_arguments()
		{
			std::vector<instruction_argument_type> arguments;
			arguments.push_back(instruction_argument_type::read_write_local);
			arguments.push_back(instruction_argument_type::read_local);
			return arguments;
		}

		std::vector<instruction_argument_type> make_constant_arguments()
		{
			std::vector<instruction_argument_type> arguments;
			arguments.push_back(instruction_argument_type::read_write_local);
			arguments.push_back(instruction_argument_type::constant);
			return arguments;
		}

		instruction_info const &get_instruction_info(instruction_type::Enum instruction)
		{
			static_assert(instruction_type::count_ == 38, "Update the 'infos' array");

			static std::array<instruction_info, instruction_type::count_> const infos =
			{{
				{"nothing", std::vector<instruction_argument_type>()},
				{"set_from_constant", make_constant_arguments()},
				{"set_null", {instruction_argument_type::read_write_local}},
				{"set_function", make_constant_arguments()},
				{"set_string", make_constant_arguments()},
				{"bind", {instruction_argument_type::read_local, instruction_argument_type::constant, instruction_argument_type::read_local}},
				{"get_bound", {instruction_argument_type::read_local, instruction_argument_type::constant, instruction_argument_type::read_write_local}},
				{"current_function", {instruction_argument_type::read_write_local}},
				{"copy", make_binary_arguments()},
				{"add", make_binary_arguments()},
				{"sub", make_binary_arguments()},
				{"mul", make_binary_arguments()},
				{"div", make_binary_arguments()},
				{"mod", make_binary_arguments()},
				{"not", {instruction_argument_type::read_write_local}},
				{"invert", {instruction_argument_type::read_write_local}},
				{"negate", {instruction_argument_type::read_write_local}},
				{"and", make_binary_arguments()},
				{"or", make_binary_arguments()},
				{"xor", make_binary_arguments()},
				{"shift_left", make_binary_arguments()},
				{"shift_right", make_binary_arguments()},
				{"shift_signed", make_binary_arguments()},
				{"equal", make_binary_arguments()},
				{"not_equal", make_binary_arguments()},
				{"less", make_binary_arguments()},
				{"less_equal", make_binary_arguments()},
				{"greater", make_binary_arguments()},
				{"greater_equal", make_binary_arguments()},
				{"call", make_constant_arguments()},
				{"call_method", {instruction_argument_type::read_write_local, instruction_argument_type::constant, instruction_argument_type::read_local}},
				{"jump", {instruction_argument_type::constant}},
				{"jump_if", {instruction_argument_type::constant, instruction_argument_type::read_local}},
				{"jump_if_not", {instruction_argument_type::constant, instruction_argument_type::read_local}},
				{"new_table", {instruction_argument_type::read_write_local}},
				{"set_element", {instruction_argument_type::read_local, instruction_argument_type::read_local, instruction_argument_type::read_local}},
				{"get_element", {instruction_argument_type::read_local, instruction_argument_type::read_local, instruction_argument_type::read_write_local}},
				{"load_module", {instruction_argument_type::read_write_local}},
			}};

			return infos[instruction];
		}

		bool is_binary_arithmetic(instruction_type::Enum instruction)
		{
			return
				(instruction >= instruction_type::add) &&
				(instruction <= instruction_type::greater_equal);
		}

		bool is_any_jump(instruction_type::Enum instruction)
		{
			using namespace instruction_type;
			switch (instruction)
			{
			case jump:
			case jump_if:
			case jump_if_not:
				return true;

			default:
				return false;
			}
		}

		instruction_type::Enum find_instruction_by_name(std::string const &name)
		{
			int i = 0;
			for (; i < instruction_type::count_; ++i)
			{
				if (name == get_instruction_info(static_cast<instruction_type::Enum>(i)).name)
				{
					break;
				}
			}

			return static_cast<instruction_type::Enum>(i);
		}


		instruction::instruction()
			: m_type(instruction_type::nothing)
		{
			m_arguments.fill(0);
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
			m_arguments.fill(0);
			m_arguments[0] = argument0;
		}

		instruction::instruction(
			instruction_type::Enum type,
			instruction_argument argument0,
			instruction_argument argument1
			)
			: m_type(type)
		{
			m_arguments.fill(0);
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
			m_arguments.fill(0);
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


		bool operator == (instruction const &left, instruction const &right)
		{
			return boost::make_tuple(left.type(), left.arguments()) == boost::make_tuple(right.type(), right.arguments());
		}
	}
}

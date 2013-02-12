#pragma once
#ifndef P0I_INSTRUCTION_HPP
#define P0I_INSTRUCTION_HPP


#include <array>
#include <string>
#include <cstdint>
#include <cstddef>


namespace p0
{
	namespace intermediate
	{
		namespace instruction_type
		{
			enum Enum
			{
				nothing,
				set_from_constant,
				set_null,
				set_function,
				set_string,
				copy,
				add,
				sub,
				mul,
				div,
				mod,
				not_,
				invert,
				negate,
				and_,
				or_,
				xor_,
				shift_left,
				shift_right,
				shift_signed,
				equal,
				not_equal,
				less,
				less_equal,
				greater,
				greater_equal,
				call,
				jump,
				jump_if,
				jump_if_not,
				new_table,
				set_element,
				get_element,
				load_module,

				count_,
				invalid_ = count_,
			};
		}

		typedef std::int64_t instruction_argument;


		struct instruction_info
		{
			char const *name;
			size_t argument_count;
		};

		instruction_info const &get_instruction_info(instruction_type::Enum instruction);
		bool is_binary_arithmetic(instruction_type::Enum instruction);
		bool is_any_jump(instruction_type::Enum instruction);
		instruction_type::Enum find_instruction_by_name(std::string const &name);


		struct instruction
		{
			enum
			{
				max_arguments = 3,
			};

			typedef std::array<instruction_argument, max_arguments> argument_array;


			instruction();
			explicit instruction(
				instruction_type::Enum type,
				argument_array const &arguments = argument_array()
				);
			explicit instruction(
				instruction_type::Enum type,
				instruction_argument argument0
				);
			explicit instruction(
				instruction_type::Enum type,
				instruction_argument argument0,
				instruction_argument argument1
				);
			explicit instruction(
				instruction_type::Enum type,
				instruction_argument argument0,
				instruction_argument argument1,
				instruction_argument argument2
				);
			instruction_type::Enum type() const;
			argument_array const &arguments() const;
			argument_array &arguments();

		private:

			instruction_type::Enum m_type;
			argument_array m_arguments;
		};
	}
}


#endif

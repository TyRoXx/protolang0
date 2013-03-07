#pragma once
#ifndef P0I_EMITTER_HPP
#define P0I_EMITTER_HPP


#include "instruction.hpp"
#include "p0common/final.hpp"
#include <vector>


namespace p0
{
	namespace intermediate
	{
		struct emitter PROTOLANG0_FINAL_CLASS
		{
			typedef std::vector<instruction> instruction_vector;


			explicit emitter(
				instruction_vector &destination
				);
			size_t get_current_jump_address() const;
			void nothing();
			void set_constant(
				instruction_argument destination,
				instruction_argument constant
				);
			void set_null(
				instruction_argument destination
				);
			void set_function(
				instruction_argument destination,
				instruction_argument function_id
				);
			void bind(
				instruction_argument closure,
				instruction_argument bound_index,
				instruction_argument source
				);
			void get_bound(
				instruction_argument closure,
				instruction_argument bound_index,
				instruction_argument destination
				);
			void current_function(instruction_argument destination);
			void set_string(
				instruction_argument destination,
				instruction_argument string_id
				);
			void copy(
				instruction_argument destination,
				instruction_argument source
				);
			void add(
				instruction_argument destination,
				instruction_argument summand
				);
			void sub(
				instruction_argument destination,
				instruction_argument source
				);
			void mul(
				instruction_argument destination,
				instruction_argument source
				);
			void div(
				instruction_argument destination,
				instruction_argument source
				);
			void mod(
				instruction_argument destination,
				instruction_argument source
				);
			void not_(
				instruction_argument destination
				);
			void invert(
				instruction_argument destination
				);
			void negate(
				instruction_argument destination
				);
			void equal(
				instruction_argument left,
				instruction_argument right);
			void not_equal(
				instruction_argument left,
				instruction_argument right);
			void less(
				instruction_argument left,
				instruction_argument right);
			void call(
				instruction_argument arguments_address,
				instruction_argument argument_count
				);
			void call_method(
				instruction_argument arguments_address,
				instruction_argument argument_count,
				instruction_argument method_name_address
				);
			void jump(
				instruction_argument destination
				);
			void jump_if(
				instruction_argument destination,
				instruction_argument condition_address
				);
			void jump_if_not(
				instruction_argument destination,
				instruction_argument condition_address
				);
			void update_jump_destination(
				size_t jump_address,
				instruction_argument destination
				);
			void new_table(
				instruction_argument destination
				);
			void set_element(
				instruction_argument table_address,
				instruction_argument key_address,
				instruction_argument value_address
				);
			void get_element(
				instruction_argument table_address,
				instruction_argument key_address,
				instruction_argument value_address
				);
			void binary_operation(
				instruction_type::Enum operation,
				instruction_argument destination,
				instruction_argument source);
			void push_instruction(
				instruction const &instruction
				);
			void load_module(
				instruction_argument name_and_result
				);

		private:

			instruction_vector &m_destination;
		};
	}
}


#endif

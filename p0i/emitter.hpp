#pragma once
#ifndef P0I_EMITTER_HPP
#define P0I_EMITTER_HPP


#include "instruction.hpp"
#include "p0common/final.hpp"
#include "p0common/types.hpp"
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
			jump_offset get_current_jump_address() const;
			void nothing();
			void set_constant(
				local_address destination,
				instruction_argument constant
				);
			void set_null(
				local_address destination
				);
			void set_function(
				local_address destination,
				instruction_argument function_id
				);
			void bind(
				local_address closure,
				instruction_argument bound_index,
				local_address source
				);
			void get_bound(
				local_address closure,
				instruction_argument bound_index,
				local_address destination
				);
			void current_function(local_address destination);
			void set_string(
				local_address destination,
				instruction_argument string_id
				);
			void copy(
				local_address destination,
				local_address source
				);
			void add(
				local_address destination,
				local_address summand
				);
			void sub(
				local_address destination,
				local_address source
				);
			void mul(
				local_address destination,
				local_address source
				);
			void div(
				local_address destination,
				local_address source
				);
			void mod(
				local_address destination,
				local_address source
				);
			void not_(
				local_address destination
				);
			void invert(
				local_address destination
				);
			void negate(
				local_address destination
				);
			void equal(
				local_address left,
				local_address right);
			void not_equal(
				local_address left,
				local_address right);
			void less(
				local_address left,
				local_address right);
			void call(
				local_address arguments_address,
				instruction_argument argument_count
				);
			void call_method(
				local_address arguments_address,
				instruction_argument argument_count,
				local_address method_name_address
				);
			void jump(
				jump_offset destination
				);
			void jump_if(
				jump_offset destination,
				local_address condition_address
				);
			void jump_if_not(
				jump_offset destination,
				local_address condition_address
				);
			void update_jump_destination(
				jump_offset jump_address,
				jump_offset destination
				);
			void new_table(
				local_address destination
				);
			void set_element(
				local_address table_address,
				local_address key_address,
				local_address value_address
				);
			void get_element(
				local_address table_address,
				local_address key_address,
				local_address value_address
				);
			void binary_operation(
				instruction_type::Enum operation,
				local_address destination,
				local_address source);
			void push_instruction(
				instruction const &instruction
				);
			void load_module(
				local_address name_and_result
				);

		private:

			instruction_vector &m_destination;
		};

		instruction make_set_from_constant(local_address destination, integer value);
	}
}


#endif

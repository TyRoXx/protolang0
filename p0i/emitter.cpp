#include "emitter.hpp"
#include "p0common/integer_cast.hpp"
#include <cassert>


namespace p0
{
	namespace intermediate
	{
		emitter::emitter(
			instruction_vector &destination
			)
			: m_destination(destination)
		{
		}

		jump_offset emitter::get_current_jump_address() const
		{
			return integer_cast<jump_offset>(m_destination.size());
		}

		void emitter::nothing()
		{
			push_instruction(instruction(
				instruction_type::nothing
				));
		}

		void emitter::set_constant(
			local_address destination,
			instruction_argument constant
			)
		{
			push_instruction(instruction(
				instruction_type::set_from_constant,
				destination,
				constant
				));
		}

		void emitter::set_null(
			local_address destination
			)
		{
			push_instruction(instruction(
				instruction_type::set_null,
				destination
				));
		}

		void emitter::set_function(
			local_address destination,
			instruction_argument function_id
			)
		{
			push_instruction(instruction(
				instruction_type::set_function,
				destination,
				function_id
				));
		}

		void emitter::bind(
			local_address closure,
			instruction_argument bound_index,
			local_address source
			)
		{
			push_instruction(instruction(
				instruction_type::bind,
				closure,
				bound_index,
				source
				));
		}

		void emitter::get_bound(
			local_address closure,
			instruction_argument bound_index,
			local_address destination
			)
		{
			push_instruction(instruction(
				instruction_type::get_bound,
				closure,
				bound_index,
				destination
				));
		}

		void emitter::current_function(local_address destination)
		{
			push_instruction(instruction(
				instruction_type::current_function,
				destination
				));
		}

		void emitter::set_string(
			local_address destination,
			instruction_argument string_id
			)
		{
			push_instruction(instruction(
				instruction_type::set_string,
				destination,
				string_id
				));
		}

		void emitter::copy(
			local_address destination,
			local_address source
			)
		{
			push_instruction(instruction(
				instruction_type::copy,
				destination,
				source
				));
		}

		void emitter::add(
			local_address destination,
			local_address summand
			)
		{
			push_instruction(instruction(
				instruction_type::add,
				destination,
				summand
				));
		}

		void emitter::sub(
			local_address destination,
			local_address source
			)
		{
			push_instruction(instruction(
				instruction_type::sub,
				destination,
				source
				));
		}

		void emitter::mul(
			local_address destination,
			local_address source
			)
		{
			push_instruction(instruction(
				instruction_type::mul,
				destination,
				source
				));
		}

		void emitter::div(
			local_address destination,
			local_address source
			)
		{
			push_instruction(instruction(
				instruction_type::div,
				destination,
				source
				));
		}

		void emitter::mod(
			local_address destination,
			local_address source
			)
		{
			push_instruction(instruction(
				instruction_type::mod,
				destination,
				source
				));
		}

		void emitter::not_(
			local_address destination
			)
		{
			push_instruction(instruction(
				instruction_type::not_,
				destination
				));
		}

		void emitter::invert(
			local_address destination
			)
		{
			push_instruction(instruction(
				instruction_type::invert,
				destination
				));
		}

		void emitter::negate(
			local_address destination
			)
		{
			push_instruction(instruction(
				instruction_type::negate,
				destination
				));
		}

		void emitter::equal(
			local_address left,
			local_address right)
		{
			push_instruction(instruction(
				instruction_type::equal,
				left,
				right
				));
		}

		void emitter::not_equal(
			local_address left,
			local_address right)
		{
			push_instruction(instruction(
				instruction_type::not_equal,
				left,
				right
				));
		}

		void emitter::less(
			local_address left,
			local_address right)
		{
			push_instruction(instruction(
				instruction_type::less,
				left,
				right
				));
		}

		void emitter::call(
			local_address arguments_address,
			instruction_argument argument_count
			)
		{
			push_instruction(instruction(
				instruction_type::call,
				arguments_address,
				argument_count
				));
		}

		void emitter::call_method(
			local_address arguments_address,
			instruction_argument argument_count,
			local_address method_name_address
			)
		{
			push_instruction(instruction(
				instruction_type::call_method,
				arguments_address,
				argument_count,
				method_name_address
				));
		}

		void emitter::jump(
			jump_offset destination
			)
		{
			push_instruction(instruction(
				instruction_type::jump,
				destination
				));
		}

		void emitter::jump_if(
			jump_offset destination,
			local_address condition_address
			)
		{
			push_instruction(instruction(
				instruction_type::jump_if,
				destination,
				condition_address
				));
		}

		void emitter::jump_if_not(
			jump_offset destination,
			local_address condition_address
			)
		{
			push_instruction(instruction(
				instruction_type::jump_if_not,
				destination,
				condition_address
				));
		}

		void emitter::update_jump_destination(
			jump_offset jump_address,
			jump_offset destination
			)
		{
			auto &jump = m_destination[integer_cast<size_t>(jump_address)];

			assert(
				jump.type() == instruction_type::jump ||
				jump.type() == instruction_type::jump_if ||
				jump.type() == instruction_type::jump_if_not
				);

			jump.arguments()[0] = destination;
		}

		void emitter::new_table(
			local_address destination
			)
		{
			push_instruction(instruction(
				instruction_type::new_table,
				destination
				));
		}

		void emitter::set_element(
			local_address table_address,
			local_address key_address,
			local_address value_address
			)
		{
			push_instruction(instruction(
				instruction_type::set_element,
				table_address,
				key_address,
				value_address
				));
		}

		void emitter::get_element(
			local_address table_address,
			local_address key_address,
			local_address value_address
			)
		{
			push_instruction(instruction(
				instruction_type::get_element,
				table_address,
				key_address,
				value_address
				));
		}

		void emitter::binary_operation(
			instruction_type::Enum operation,
			local_address destination,
			local_address source)
		{
			assert(is_binary_arithmetic(operation));

			push_instruction(instruction(
				operation,
				destination,
				source));
		}

		void emitter::load_module(
			local_address name_and_result
			)
		{
			push_instruction(instruction(
				instruction_type::load_module,
				name_and_result
				));
		}


		void emitter::push_instruction(
			instruction const &instruction
			)
		{
			m_destination.push_back(instruction);
		}

		instruction make_set_from_constant(local_address destination, integer value)
		{
			//TODO optimize
			std::vector<instruction> v;
			emitter e(v);
			e.set_constant(destination, value);
			return v.front();
		}
	}
}

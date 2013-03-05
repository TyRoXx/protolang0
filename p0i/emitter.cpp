#include "emitter.hpp"
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

		size_t emitter::get_current_jump_address() const
		{
			return m_destination.size();
		}

		void emitter::nothing()
		{
			push_instruction(instruction(
				instruction_type::nothing
				));
		}

		void emitter::set_constant(
			instruction_argument destination,
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
			instruction_argument destination
			)
		{
			push_instruction(instruction(
				instruction_type::set_null,
				destination
				));
		}

		void emitter::set_function(
			instruction_argument destination,
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
			instruction_argument closure,
			instruction_argument bound_index,
			instruction_argument source
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
			instruction_argument closure,
			instruction_argument bound_index,
			instruction_argument destination
			)
		{
			push_instruction(instruction(
				instruction_type::get_bound,
				closure,
				bound_index,
				destination
				));
		}

		void emitter::current_function(instruction_argument destination)
		{
			push_instruction(instruction(
				instruction_type::current_function,
				destination
				));
		}

		void emitter::set_string(
			instruction_argument destination,
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
			instruction_argument destination,
			instruction_argument source
			)
		{
			push_instruction(instruction(
				instruction_type::copy,
				destination,
				source
				));
		}

		void emitter::add(
			instruction_argument destination,
			instruction_argument summand
			)
		{
			push_instruction(instruction(
				instruction_type::add,
				destination,
				summand
				));
		}

		void emitter::sub(
			instruction_argument destination,
			instruction_argument source
			)
		{
			push_instruction(instruction(
				instruction_type::sub,
				destination,
				source
				));
		}

		void emitter::mul(
			instruction_argument destination,
			instruction_argument source
			)
		{
			push_instruction(instruction(
				instruction_type::mul,
				destination,
				source
				));
		}

		void emitter::div(
			instruction_argument destination,
			instruction_argument source
			)
		{
			push_instruction(instruction(
				instruction_type::div,
				destination,
				source
				));
		}

		void emitter::mod(
			instruction_argument destination,
			instruction_argument source
			)
		{
			push_instruction(instruction(
				instruction_type::mod,
				destination,
				source
				));
		}

		void emitter::not_(
			instruction_argument destination
			)
		{
			push_instruction(instruction(
				instruction_type::not_,
				destination
				));
		}

		void emitter::invert(
			instruction_argument destination
			)
		{
			push_instruction(instruction(
				instruction_type::invert,
				destination
				));
		}

		void emitter::negate(
			instruction_argument destination
			)
		{
			push_instruction(instruction(
				instruction_type::negate,
				destination
				));
		}

		void emitter::equal(
			instruction_argument left,
			instruction_argument right)
		{
			push_instruction(instruction(
				instruction_type::equal,
				left,
				right
				));
		}

		void emitter::not_equal(
			instruction_argument left,
			instruction_argument right)
		{
			push_instruction(instruction(
				instruction_type::not_equal,
				left,
				right
				));
		}

		void emitter::less(
			instruction_argument left,
			instruction_argument right)
		{
			push_instruction(instruction(
				instruction_type::less,
				left,
				right
				));
		}

		void emitter::call(
			instruction_argument arguments_address,
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
			instruction_argument arguments_address,
				instruction_argument argument_count,
			instruction_argument method_name_address
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
			instruction_argument destination
			)
		{
			push_instruction(instruction(
				instruction_type::jump,
				destination
				));
		}

		void emitter::jump_if(
			instruction_argument destination,
			instruction_argument condition_address
			)
		{
			push_instruction(instruction(
				instruction_type::jump_if,
				destination,
				condition_address
				));
		}

		void emitter::jump_if_not(
			instruction_argument destination,
			instruction_argument condition_address
			)
		{
			push_instruction(instruction(
				instruction_type::jump_if_not,
				destination,
				condition_address
				));
		}

		void emitter::update_jump_destination(
			size_t jump_address,
			instruction_argument destination
			)
		{
			auto &jump = m_destination[jump_address];

			assert(
				jump.type() == instruction_type::jump ||
				jump.type() == instruction_type::jump_if ||
				jump.type() == instruction_type::jump_if_not
				);

			jump.arguments()[0] = destination;
		}

		void emitter::new_table(
			instruction_argument destination
			)
		{
			push_instruction(instruction(
				instruction_type::new_table,
				destination
				));
		}

		void emitter::set_element(
			instruction_argument table_address,
			instruction_argument key_address,
			instruction_argument value_address
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
			instruction_argument table_address,
			instruction_argument key_address,
			instruction_argument value_address
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
			instruction_argument destination,
			instruction_argument source)
		{
			assert(is_binary_arithmetic(operation));

			push_instruction(instruction(
				operation,
				destination,
				source));
		}

		void emitter::load_module(
			instruction_argument name_and_result
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
	}
}

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

		void emitter::return_()
		{
			push_instruction(instruction(
				instruction_type::return_
				));
		}

		void emitter::set_from_constant(
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

		void emitter::allocate(
			instruction_argument size
			)
		{
			push_instruction(instruction(
				instruction_type::allocate,
				size
				));
		}

		void emitter::deallocate(
			instruction_argument size
			)
		{
			push_instruction(instruction(
				instruction_type::deallocate,
				size
				));
		}

		void emitter::call(
			instruction_argument argument_count
			)
		{
			push_instruction(instruction(
				instruction_type::call,
				argument_count
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

		void emitter::update_jump_destination(
			size_t jump_address,
			instruction_argument destination
			)
		{
			auto &jump = m_destination[jump_address];

			assert(
				jump.type() == instruction_type::jump ||
				jump.type() == instruction_type::jump_if
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


		void emitter::push_instruction(
			instruction const &instruction
			)
		{
			m_destination.push_back(instruction);
		}
	}
}

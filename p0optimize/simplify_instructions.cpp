#include "simplify_instructions.hpp"
#include "analyzation.hpp"
#include <p0i/emitter.hpp>
#include <p0run/value.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <unordered_map>

namespace p0
{
	struct local_constant_tracker
	{
		void handle_incoming_jump()
		{
			m_known_values.clear();
		}

		void handle_instruction(intermediate::instruction const &instruction)
		{
			switch (instruction.type())
			{
			case p0::intermediate::instruction_type::set_from_constant:
				{
					m_known_values[static_cast<local_address>(instruction.arguments()[0])] = instruction.arguments()[1];
					break;
				}

			case p0::intermediate::instruction_type::copy:
				{
					auto const destination = static_cast<local_address>(instruction.arguments()[0]);
					auto const source = instruction.arguments()[1];
					auto const known_source = m_known_values.find(source);
					if (known_source == m_known_values.end())
					{
						m_known_values.erase(destination);
					}
					else
					{
						m_known_values[destination] = known_source->second;
					}
					break;
				}

			case p0::intermediate::instruction_type::add:
				{
					break;
				}

				//TODO erase from known_locals on write operations
			}
		}

		boost::optional<integer> find_current_value(local_address where)
		{
			auto const i = m_known_values.find(where);
			if (i == m_known_values.end())
			{
				return boost::none;
			}
			return i->second;
		}

	private:

		std::unordered_map<local_address, integer> m_known_values;
	};

	p0::intermediate::function::instruction_vector simplify_instructions(p0::intermediate::function::instruction_vector const &original)
	{
		p0::intermediate::function::instruction_vector optimized = original;
		local_constant_tracker known_locals;
		auto const jump_destinations = find_all_jump_destinations(original);
		for (std::size_t i = 0; i < optimized.size(); ++i)
		{
			if (jump_destinations.find(i) != jump_destinations.end())
			{
				known_locals.handle_incoming_jump();
			}

			auto &instruction = optimized[i];
			known_locals.handle_instruction(instruction);

			switch (instruction.type())
			{
			case p0::intermediate::instruction_type::copy:
				{
					auto const source = static_cast<local_address>(instruction.arguments()[1]);
					auto const value = known_locals.find_current_value(source);
					if (value)
					{
						auto const destination = static_cast<local_address>(instruction.arguments()[0]);
						instruction = intermediate::make_set_from_constant(destination, *value);
					}
					break;
				}

			case p0::intermediate::instruction_type::jump_if:
			case p0::intermediate::instruction_type::jump_if_not:
				{
					auto const condition = static_cast<local_address>(instruction.arguments()[1]);
					auto const value = known_locals.find_current_value(condition);
					if (value)
					{
						auto const destination = instruction.arguments()[0];
						bool const value_for_jump = (instruction.type() == p0::intermediate::instruction_type::jump_if);
						if (run::to_boolean(run::value(*value)) == value_for_jump)
						{
							instruction = intermediate::instruction(intermediate::instruction_type::jump, destination);
						}
						else
						{
							instruction = intermediate::instruction(intermediate::instruction_type::nothing);
						}
					}
					break;
				}

			default:
				break;
			}
		}
		return optimized;
	}
}

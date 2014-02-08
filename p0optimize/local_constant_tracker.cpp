#include "local_constant_tracker.hpp"
#include <boost/foreach.hpp>

namespace p0
{
	void local_constant_tracker::handle_incoming_jump()
	{
		m_known_values.clear();
	}

	void local_constant_tracker::handle_instruction(intermediate::instruction const &instruction)
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

	boost::optional<integer> local_constant_tracker::find_current_value(local_address where)
	{
		auto const i = m_known_values.find(where);
		if (i == m_known_values.end())
		{
			return boost::none;
		}
		return i->second;
	}
}

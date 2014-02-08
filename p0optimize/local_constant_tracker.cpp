#include "local_constant_tracker.hpp"
#include <boost/foreach.hpp>

namespace p0
{
	void local_constant_tracker::invalidate()
	{
		m_known_values.clear();
	}

	void local_constant_tracker::update_value(local_address where, integer value)
	{
		m_known_values[where] = value;
	}

	void local_constant_tracker::set_unknown(local_address where)
	{
		m_known_values.erase(where);
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


	void track_instruction(local_constant_tracker &locals, intermediate::instruction const &instruction)
	{
		switch (instruction.type())
		{
		case p0::intermediate::instruction_type::set_from_constant:
			{
				locals.update_value(static_cast<local_address>(instruction.arguments()[0]), instruction.arguments()[1]);
				break;
			}

		case p0::intermediate::instruction_type::copy:
			{
				auto const destination = static_cast<local_address>(instruction.arguments()[0]);
				auto const source = static_cast<local_address>(instruction.arguments()[1]);
				auto const known_source = locals.find_current_value(source);
				if (known_source)
				{
					locals.update_value(destination, *known_source);
				}
				else
				{
					locals.set_unknown(destination);
				}
				break;
			}

		case p0::intermediate::instruction_type::add:
			{
				auto const destination = static_cast<local_address>(instruction.arguments()[0]);
				auto const known_destination = locals.find_current_value(destination);

				auto const source = static_cast<local_address>(instruction.arguments()[1]);
				auto const known_source = locals.find_current_value(source);

				if (known_source &&
					known_destination)
				{
					//TODO: check overflow
					auto const result = (*known_source + *known_destination);
					locals.update_value(destination, result);
				}
				else
				{
					locals.set_unknown(destination);
				}
				break;
			}

		default:
			{
				auto const &info = intermediate::get_instruction_info(instruction.type());
				for (std::size_t i = 0; i < info.arguments.size(); ++i)
				{
					switch (info.arguments[i])
					{
					case intermediate::instruction_argument_type::read_write_local:
						locals.set_unknown(static_cast<local_address>(instruction.arguments()[i]));
						break;

					case intermediate::instruction_argument_type::read_local:
					case intermediate::instruction_argument_type::constant:
						break;
					}
				}
				break;
			}
		}
	}
}

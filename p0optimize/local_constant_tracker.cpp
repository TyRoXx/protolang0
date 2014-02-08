#include "local_constant_tracker.hpp"
#include <boost/foreach.hpp>

namespace p0
{
	void local_constant_tracker::handle_incoming_jump()
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
}

#include "table.hpp"


namespace p0
{
	namespace run
	{
		void table::set_element(value const &key, value const &value)
		{
			m_elements[key] = value;
		}

		value table::get_element(value const &key) const
		{
			auto const e = m_elements.find(key);
			if (e != m_elements.end())
			{
				return e->second;
			}
			return value();
		}
	}
}

#include "table.hpp"


namespace p0
{
	namespace run
	{
		void table::mark_recursively()
		{
			if (is_marked())
			{
				return;
			}
			for (auto i = m_elements.begin(); i != m_elements.end(); ++i)
			{
				auto const &value = i->second;
				if (value.type == value_type::object)
				{
					value.obj->mark_recursively();
				}
			}
		}

		boost::optional<value> table::get_element(value const &key) const
		{
			auto const e = m_elements.find(key);
			if (e != m_elements.end())
			{
				return e->second;
			}
			return value();
		}

		bool table::set_element(value const &key, value const &value)
		{
			m_elements[key] = value;
			return true;
		}
	}
}

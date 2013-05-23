#include "table.hpp"
#include <boost/foreach.hpp>


namespace p0
{
	namespace run
	{
		table::table()
		{
		}

		table::table(elements elements)
			: m_elements(std::move(elements))
		{
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

		void table::print(std::ostream &out) const
		{
			out << "[";

			BOOST_FOREACH (auto const &element, m_elements)
			{
				out << element.first << ": " << element.second << ", ";
			}

			out << "]";
		}


		void table::mark_recursively()
		{
			for (auto i = m_elements.begin(); i != m_elements.end(); ++i)
			{
				auto const &value = i->second;
				if (value.type == value_type::object)
				{
					value.obj->mark();
				}
			}
		}
	}
}

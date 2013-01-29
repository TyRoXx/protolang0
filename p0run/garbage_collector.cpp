#include "garbage_collector.hpp"
#include <algorithm>
#include <functional>


namespace p0
{
	namespace run
	{
		void garbage_collector::add_object(std::unique_ptr<object> object)
		{
			m_objects.push_back(std::move(object));
		}

		void garbage_collector::mark()
		{
			for (auto i = m_objects.begin(); i != m_objects.end(); ++i)
			{
				(*i)->unmark();
			}

			for (auto i = m_objects.begin(); i != m_objects.end(); ++i)
			{
				(*i)->mark();
			}
		}

		void garbage_collector::sweep()
		{
			auto const is_marked = std::bind(&object::is_marked, std::placeholders::_1);
			auto const new_end = std::partition(m_objects.begin(),
												m_objects.end(),
												is_marked);
			m_objects.erase(new_end, m_objects.end());
		}
	}
}

#include "object.hpp"


namespace p0
{
	namespace run
	{
		object::object()
			: m_is_marked(false)
		{
		}

		object::~object()
		{
		}

		void object::mark()
		{
			if (m_is_marked)
			{
				return;
			}
			m_is_marked = true;
			mark_recursively();
		}

		void object::unmark()
		{
			m_is_marked = false;
		}

		bool object::is_marked() const
		{
			return m_is_marked;
		}
	}
}

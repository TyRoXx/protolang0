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

		integer object::get_hash_code() const
		{
			return static_cast<integer>(std::hash<object const *>()(this));
		}

		bool object::equals(object const &other) const
		{
			return (this == &other);
		}
	}
}

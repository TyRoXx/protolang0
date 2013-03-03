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

		boost::optional<value> object::get_element(value const & /*key*/) const
		{
			return boost::optional<value>();
		}

		bool object::set_element(value const & /*key*/, value const & /*value*/)
		{
			return false;
		}

		integer object::get_hash_code() const
		{
			return static_cast<integer>(std::hash<object const *>()(this));
		}

		bool object::equals(object const &other) const
		{
			return (this == &other);
		}

		comparison_result::Enum object::compare(object const &right) const
		{
			if (this < &right)
			{
				return comparison_result::less;
			}
			if (this > &right)
			{
				return comparison_result::greater;
			}
			return comparison_result::equal;
		}

		boost::optional<value> object::call(
			std::vector<value> const & /*arguments*/,
			interpreter & /*interpreter*/)
		{
			return boost::optional<value>();
		}

		void object::print(std::ostream &out) const
		{
			out << "object";
		}

		bool object::bind(size_t /*index*/, value const & /*value*/)
		{
			return false;
		}

		boost::optional<value> object::get_bound(size_t /*index*/) const
		{
			return boost::optional<value>();
		}
	}
}

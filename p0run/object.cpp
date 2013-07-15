#include "object.hpp"
#include <stdexcept>


namespace p0
{
	namespace run
	{
		object_element_callback::~object_element_callback()
		{
		}


		object::object()
			: m_is_marked(false)
		{
		}

		object::~object()
		{
		}

		void object::mark()
		{
			//m_is_marked_permanently is not a condition here because we have
			//to mark the children anyway
			if (m_is_marked)
			{
				return;
			}

			//mark this object before the children
			//to prevent endless recursion on circular references
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

		void object::enumerate_elements(object_element_callback &) const
		{
			//no elements by default, nothing to do
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

		boost::optional<value> object::call_method(
				value const & /*method_name*/,
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

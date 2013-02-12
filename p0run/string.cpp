#include "string.hpp"


namespace p0
{
	namespace run
	{
		string::string(std::string content)
			: m_content(std::move(content))
		{
		}

		std::string const &string::content() const
		{
			return m_content;
		}

		boost::optional<value> string::get_element(value const &key) const
		{
			if (key.type == value_type::integer &&
				key.i >= 0 &&
				static_cast<integer>(m_content.size()) > key.i)
			{
				return value(static_cast<integer>(
					m_content[static_cast<size_t>(key.i)]));
			}
			return boost::optional<value>();
		}

		bool string::set_element(value const & /*key*/, value const & /*value*/)
		{
			return false;
		}

		integer string::get_hash_code() const
		{
			return static_cast<integer>(std::hash<std::string>()(m_content));
		}

		bool string::equals(object const &other) const
		{
			if (string const * const other_string = dynamic_cast<string const *>(&other))
			{
				return (other_string->m_content == m_content);
			}
			return false;
		}

		comparison_result::Enum string::compare(object const &right) const
		{
			if (string const * const right_string = dynamic_cast<string const *>(&right))
			{
				int const result = m_content.compare(right_string->m_content);
				if (result < 0)
				{
					return comparison_result::less;
				}
				if (result > 0)
				{
					return comparison_result::greater;
				}
				return comparison_result::equal;
			}
			return object::compare(right);
		}

		void string::print(std::ostream &out) const
		{
			out << m_content;
		}


		void string::mark_recursively()
		{
		}
	}
}

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

		void string::mark_recursively()
		{
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

		bool string::set_element(value const &key, value const &value)
		{
			return false;
		}
	}
}

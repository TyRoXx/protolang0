#include "string.hpp"


namespace p0
{
	namespace run
	{
		string::string(std::string content)
			: m_content(std::move(content))
		{
		}

		void string::mark_recursively()
		{
		}

		boost::optional<value> string::get_element(value const &key) const
		{
			return boost::optional<value>();
		}

		bool string::set_element(value const &key, value const &value)
		{
			return false;
		}
	}
}

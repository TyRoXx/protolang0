#include "unit.hpp"


namespace p0
{
	namespace intermediate
	{
		unit::unit(
			function_vector functions,
			string_vector strings,
			size_t integer_width
			)
			: m_functions(std::move(functions))
			, m_strings(std::move(strings))
			, m_integer_width(integer_width)
		{
		}

		unit::function_vector const &unit::functions() const
		{
			return m_functions;
		}

		unit::string_vector const &unit::strings() const
		{
			return m_strings;
		}

		size_t unit::integer_width() const
		{
			return m_integer_width;
		}
	}
}

#include "unit.hpp"


namespace p0
{
	namespace intermediate
	{
		unit::unit(
			function_vector functions,
			string_vector strings
			)
			: m_functions(std::move(functions))
			, m_strings(std::move(strings))
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
	}
}

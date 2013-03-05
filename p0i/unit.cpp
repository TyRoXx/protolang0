#include "unit.hpp"


namespace p0
{
	namespace intermediate
	{
		unit::unit(
			function_vector functions,
			string_vector strings,
			std::unique_ptr<unit_info> info
			)
			: m_functions(std::move(functions))
			, m_strings(std::move(strings))
			, m_info(std::move(info))
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

		unit_info const *unit::info() const
		{
			return m_info.get();
		}
	}
}

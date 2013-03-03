#include "function.hpp"


namespace p0
{
	namespace intermediate
	{
		function::function()
			: m_parameters(0)
			, m_bound_variables(0)
		{
		}

		function::function(
			instruction_vector body,
			size_t parameters,
			size_t bound_variables
			)
			: m_body(std::move(body))
			, m_parameters(parameters)
			, m_bound_variables(bound_variables)
		{
		}

		function::instruction_vector const &function::body() const
		{
			return m_body;
		}

		size_t function::parameters() const
		{
			return m_parameters;
		}

		size_t function::bound_variables() const
		{
			return m_bound_variables;
		}
	}
}

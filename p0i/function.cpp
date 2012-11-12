#include "function.hpp"


namespace p0
{
	namespace intermediate
	{
		function::function()
			: m_parameters(0)
		{
		}

		function::function(
			instruction_vector body,
			size_t parameters
			)
			: m_body(std::move(body))
			, m_parameters(parameters)
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
	}
}

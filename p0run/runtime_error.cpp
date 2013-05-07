#include "runtime_error.hpp"


namespace p0
{
	namespace run
	{
		runtime_error::runtime_error(
				runtime_error_code::type type,
				intermediate::function_ref const &function,
				std::size_t instruction
				)
			: m_type(type)
			, m_function(function)
			, m_instruction(instruction)
		{
		}

		runtime_error_code::type runtime_error::type() const
		{
			return m_type;
		}

		intermediate::function_ref const &runtime_error::function() const
		{
			return m_function;
		}

		std::size_t runtime_error::instruction() const
		{
			return m_instruction;
		}
	}
}

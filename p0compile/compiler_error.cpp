#include "compiler_error.hpp"


namespace p0
{
	compiler_error::compiler_error(
		std::string const &message,
		source_range position
		)
		: std::runtime_error(message)
		, m_position(position)
	{
	}

	source_range const &compiler_error::position() const
	{
		return m_position;
	}
}

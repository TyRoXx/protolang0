#include "runtime_error_exception.hpp"


namespace p0
{
	namespace run
	{
		runtime_error_exception::runtime_error_exception(run::runtime_error error)
			: std::runtime_error("p0 interpreter runtime error")
			, m_error(std::move(error))
		{
		}

		run::runtime_error const &runtime_error_exception::error() const
		{
			return m_error;
		}
	}
}

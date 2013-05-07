#pragma once
#ifndef P0RUN_RUNTIME_ERROR_EXCEPTION_HPP
#define P0RUN_RUNTIME_ERROR_EXCEPTION_HPP


#include "runtime_error.hpp"
#include <stdexcept>


namespace p0
{
	namespace run
	{
		struct runtime_error_exception : std::runtime_error
		{
			runtime_error_exception(run::runtime_error error);
			run::runtime_error const &error() const;

		private:

			run::runtime_error m_error;
		};
	}
}


#endif

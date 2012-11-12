#pragma once
#ifndef P0C_COMPILER_ERROR_HPP
#define P0C_COMPILER_ERROR_HPP


#include "source_range.hpp"
#include <stdexcept>


namespace p0
{
	struct compiler_error : std::runtime_error
	{
		explicit compiler_error(
			std::string const &message,
			source_range position
			);
		source_range const &position() const;

	private:

		source_range m_position;
	};
}


#endif

#pragma once
#ifndef P0C_PRETTY_PRINT_ERROR_HPP
#define P0C_PRETTY_PRINT_ERROR_HPP


#include "source_range.hpp"
#include <ostream>


namespace p0
{
	struct compiler_error;


	void pretty_print_error(
		std::ostream &out,
		p0::source_range source,
		p0::compiler_error const &error
		);
}


#endif

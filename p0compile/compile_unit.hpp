#pragma once
#ifndef P0C_COMPILE_UNIT_HPP
#define P0C_COMPILE_UNIT_HPP


#include "p0i/unit.hpp"
#include "source_range.hpp"


namespace p0
{
	intermediate::unit compile_unit(source_range const &source);
	intermediate::unit compile_unit_from_file(std::string const &file_name);
}


#endif

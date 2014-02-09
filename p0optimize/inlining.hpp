#pragma once
#ifndef P0OPTIMIZE_INLINING_HPP
#define P0OPTIMIZE_INLINING_HPP


#include "p0i/function.hpp"


namespace p0
{
	p0::intermediate::function::instruction_vector inlining(
	        p0::intermediate::function::instruction_vector const &original,
	        std::vector<p0::intermediate::function> const &environmental_functions);
}


#endif

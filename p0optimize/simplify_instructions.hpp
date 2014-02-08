#pragma once
#ifndef P0OPTIMIZE_SIMPLIFY_INSTRUCTIONS_HPP
#define P0OPTIMIZE_SIMPLIFY_INSTRUCTIONS_HPP


#include "p0i/function.hpp"

namespace p0
{
	p0::intermediate::function::instruction_vector simplify_instructions(p0::intermediate::function::instruction_vector const &original);
}


#endif

#pragma once
#ifndef P0OPTIMIZE_CONSTANT_FOLDING_HPP
#define P0OPTIMIZE_CONSTANT_FOLDING_HPP


#include "p0i/function.hpp"


namespace p0
{
	p0::intermediate::function::instruction_vector fold_constants(p0::intermediate::function const &original);
}


#endif

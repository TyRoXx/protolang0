#pragma once
#ifndef P0OPTIMIZE_REMOVE_DEAD_CODE_HPP
#define P0OPTIMIZE_REMOVE_DEAD_CODE_HPP


#include "p0i/function.hpp"


namespace p0
{
	p0::intermediate::function::instruction_vector remove_dead_code(p0::intermediate::function::instruction_vector const &original);
}


#endif

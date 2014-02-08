#pragma once
#ifndef P0OPTIMIZE_REMOVE_NO_OPS_HPP
#define P0OPTIMIZE_REMOVE_NO_OPS_HPP


#include "p0i/function.hpp"


namespace p0
{
	p0::intermediate::function::instruction_vector remove_no_ops(p0::intermediate::function::instruction_vector const &original);
}


#endif

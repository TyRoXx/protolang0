#pragma once
#ifndef P0OPTIMIZE_TRANSFORMATION_HPP
#define P0OPTIMIZE_TRANSFORMATION_HPP


#include "p0i/function.hpp"
#include <boost/unordered_set.hpp>


namespace p0
{
	p0::intermediate::function::instruction_vector remove_instructions_at_positions(
			p0::intermediate::function::instruction_vector const &original,
			boost::unordered_set<std::size_t> const &positions_to_remove);
}


#endif

#pragma once
#ifndef P0OPTIMIZE_ANALYZATION_HPP
#define P0OPTIMIZE_ANALYZATION_HPP


#include "p0i/function.hpp"
#include <boost/unordered_set.hpp>


namespace p0
{
	boost::unordered_set<std::size_t> find_all_jump_destinations(p0::intermediate::function::instruction_vector const &code);
}


#endif

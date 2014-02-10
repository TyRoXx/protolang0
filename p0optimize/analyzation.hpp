#pragma once
#ifndef P0OPTIMIZE_ANALYZATION_HPP
#define P0OPTIMIZE_ANALYZATION_HPP


#include "p0i/function.hpp"
#include <unordered_set>
#include <boost/range/iterator_range.hpp>


namespace p0
{
	std::unordered_set<std::size_t> find_all_jump_destinations(p0::intermediate::function::instruction_vector const &code);

	typedef boost::iterator_range<p0::intermediate::instruction const *> const_instruction_range;

	typedef std::size_t section_id;

	static section_id const calling_section = ~static_cast<section_id>(0);

	struct linear_section
	{
		const_instruction_range code;
		std::unordered_set<section_id> destinations;
		std::unordered_set<section_id> origins;
	};

	struct function_section_graph
	{
		std::vector<linear_section> sections;
	};

	function_section_graph dissect_function(p0::intermediate::function::instruction_vector const &code);
}


#endif

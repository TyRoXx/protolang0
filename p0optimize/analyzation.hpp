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

	struct linear_section
	{
		const_instruction_range code;
		std::vector<section_id> destinations;
	};

	struct function_section_graph
	{
		std::vector<linear_section> sections;
	};

	struct instruction_info
	{
		section_id preliminary_section;
		bool is_jump_destination;

		explicit instruction_info(section_id preliminary_section)
			: preliminary_section(preliminary_section)
			, is_jump_destination(false)
		{
		}
	};

	function_section_graph dissect_function(p0::intermediate::function::instruction_vector const &code);
}


#endif

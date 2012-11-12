#pragma once
#ifndef P0I_PTREE_LOAD_UNIT_HPP
#define P0I_PTREE_LOAD_UNIT_HPP


#include <boost/property_tree/ptree.hpp>


namespace p0
{
	namespace intermediate
	{
		struct unit;


		unit load_unit(
			boost::property_tree::ptree const &tree
			);
	}
}


#endif

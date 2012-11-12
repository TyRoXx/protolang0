#pragma once
#ifndef P0I_PTREE_SAVE_UNIT_HPP
#define P0I_PTREE_SAVE_UNIT_HPP


#include <boost/property_tree/ptree.hpp>


namespace p0
{
	namespace intermediate
	{
		struct unit;


		void save_unit(
			boost::property_tree::ptree &tree,
			unit const &unit
			);
	}
}


#endif

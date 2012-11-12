#pragma once
#ifndef P0I_SAVE_UNIT_HPP
#define P0I_SAVE_UNIT_HPP


#include <ostream>


namespace p0
{
	namespace intermediate
	{
		struct unit;


		void save_unit(
			std::ostream &file,
			unit const &unit
			);
	}
}


#endif

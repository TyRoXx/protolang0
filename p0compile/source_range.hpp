#pragma once
#ifndef P0C_SOURCE_RANGE_HPP
#define P0C_SOURCE_RANGE_HPP


#include <boost/range/iterator_range.hpp>
#include <string>


namespace p0
{
	typedef boost::iterator_range<char const *> source_range;


	namespace
	{
		std::string source_range_to_string(source_range source)
		{
			return std::string(
				source.begin(),
				source.end()
				);
		}
	}
}


#endif

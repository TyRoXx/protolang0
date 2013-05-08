#pragma once
#ifndef P0COMMON_CHECKED_CAST_HPP
#define P0COMMON_CHECKED_CAST_HPP


#include <cassert>


namespace p0
{
	template <class To, class From>
	To integer_cast(From value)
	{
		auto const converted = static_cast<To>(value);
		assert(static_cast<From>(converted) == value);
		return converted;
	}
}


#endif

#pragma once
#ifndef P0RT_STD_MODULE_HPP
#define P0RT_STD_MODULE_HPP


#include "p0run/value.hpp"


namespace p0
{
	namespace run
	{
		struct garbage_collector;
	}

	namespace rt
	{
		run::value register_standard_module(run::garbage_collector &gc);
	}
}


#endif

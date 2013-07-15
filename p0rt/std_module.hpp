#pragma once
#ifndef P0RT_STD_MODULE_HPP
#define P0RT_STD_MODULE_HPP


#include "p0run/value.hpp"


namespace p0
{
	namespace run
	{
		struct interpreter;
	}

	namespace rt
	{
		run::value register_standard_module(run::interpreter &interpreter);
	}
}


#endif

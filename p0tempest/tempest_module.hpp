#pragma once
#ifndef P0TEMPEST_TEMPEST_MODULE_HPP
#define P0TEMPEST_TEMPEST_MODULE_HPP


#include "p0run/value.hpp"


namespace p0
{
	namespace run
	{
		struct interpreter;
	}

	namespace tempest
	{
		extern std::string const default_module_name;
		run::value register_tempest_module(run::interpreter &interpreter);
	}
}


#endif

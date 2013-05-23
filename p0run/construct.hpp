#pragma once
#ifndef P0RUN_CONSTRUCT_HPP
#define P0RUN_CONSTRUCT_HPP


#include "raw_storage.hpp"


namespace p0
{
	namespace run
	{
		template <class T, class ...Args>
		object &construct(garbage_collector &gc, Args && ...args)
		{
			return raw_storage(gc, sizeof(T)).construct<T>(std::forward<Args>(args)...);
		}
	}
}


#endif

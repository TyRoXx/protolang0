#pragma once
#ifndef P0RT_INSERT_HPP
#define P0RT_INSERT_HPP


#include "p0run/object.hpp"
#include "expose.hpp"


namespace p0
{
	namespace rt
	{
		inline void insert(run::object &table,
					run::interpreter &interpreter,
					std::string key,
					run::value const &element)
		{
			if (!table.set_element(
				run::value(expose(interpreter, std::move(key))),
				element
				))
			{
				//TODO better error handling
				throw std::runtime_error("Cannot set element in table");
			}
		}
	}
}


#endif

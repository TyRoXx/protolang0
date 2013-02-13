#pragma once
#ifndef P0RT_INSERT_HPP
#define P0RT_INSERT_HPP


#include "p0run/object.hpp"
#include "expose.hpp"


namespace p0
{
	namespace rt
	{
		void insert(run::object &table,
					run::interpreter &interpreter,
					std::string key,
					run::value const &element);

		void insert(run::object &table,
					run::interpreter &interpreter,
					std::string key,
					run::integer element);

		void insert(run::object &table,
					run::interpreter &interpreter,
					std::string key,
					std::string element);

		template <class Functor>
		void insert(run::object &table,
					run::interpreter &interpreter,
					std::string key,
					function_tag,
					Functor &&element)
		{
			return insert(table,
				   interpreter,
				   std::move(key),
				   expose(interpreter, function_tag(), std::forward<Functor>(element)));
		}
	}
}


#endif

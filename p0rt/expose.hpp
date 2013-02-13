#pragma once
#ifndef P0RT_EXPOSE_HPP
#define P0RT_EXPOSE_HPP


#include "p0run/string.hpp"
#include "p0run/interpreter.hpp"
#include "native_function.hpp"


namespace p0
{
	namespace rt
	{
		template <class F>
		run::object &expose_fn(run::interpreter &interpreter, F &&functor)
		{
			return interpreter.register_object(
						make_function(std::forward<F>(functor)));
		}

		run::object &expose(run::interpreter &interpreter, std::string content)
		{
			return interpreter.register_object(
						std::unique_ptr<run::object>(new run::string(std::move(content))));
		}
	}
}


#endif

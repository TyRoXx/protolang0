#pragma once
#ifndef P0RT_EXPOSE_HPP
#define P0RT_EXPOSE_HPP


#include "p0run/string.hpp"
#include "p0run/table.hpp"
#include "p0run/interpreter.hpp"
#include "native_function.hpp"


namespace p0
{
	namespace rt
	{
		struct function_tag {};


		template <class F>
		run::value expose_fn(run::interpreter &interpreter, F &&functor)
		{
			return run::value(interpreter.register_object(
						make_function(std::forward<F>(functor))));
		}

		template <class F>
		run::value expose(run::interpreter &interpreter, function_tag, F &&functor)
		{
			return expose_fn(interpreter, std::forward<F>(functor));
		}

		run::value expose(run::interpreter &interpreter, std::string content);

		run::value expose(run::interpreter &interpreter, run::table::elements content);
	}
}


#endif

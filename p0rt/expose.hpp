#pragma once
#ifndef P0RT_EXPOSE_HPP
#define P0RT_EXPOSE_HPP


#include "p0run/string.hpp"
#include "p0run/table.hpp"
#include "p0run/garbage_collector.hpp"
#include "native_function.hpp"


namespace p0
{
	namespace rt
	{
		struct function_tag {};


		template <class F>
		run::value expose_fn(run::garbage_collector &gc, F &&functor)
		{
			typedef typename std::decay<F>::type clean_functor_type;
			typedef native_function<clean_functor_type> wrapper;

			return run::value(
				run::construct_object<wrapper>(
							gc, std::forward<F>(functor)));
		}

		template <class F>
		run::value expose(run::garbage_collector &gc, function_tag, F &&functor)
		{
			return expose_fn(gc, std::forward<F>(functor));
		}

		run::value expose(run::garbage_collector &gc, std::string content);

		run::value expose(run::garbage_collector &gc, run::table::elements content);
	}
}


#endif

#pragma once
#ifndef P0RT_NATIVE_FUNCTION_HPP
#define P0RT_NATIVE_FUNCTION_HPP


#include "p0run/object.hpp"
#include <memory>


namespace p0
{
	namespace rt
	{
		template <class F>
		struct native_function PROTOLANG0_FINAL_CLASS : run::object
		{
			template <class G>
			explicit native_function(G &&functor)
				: m_functor(std::forward<G>(functor))
			{
			}

			virtual boost::optional<run::value> call(
				std::vector<run::value> const &arguments,
				run::interpreter & /*interpreter*/) PROTOLANG0_FINAL_METHOD
			{
				return m_functor(arguments);
			}

		private:

			F const m_functor;


			virtual void mark_recursively() override
			{
			}
		};
	}
}


#endif

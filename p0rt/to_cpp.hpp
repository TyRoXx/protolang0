#pragma once
#ifndef P0RT_TO_CPP_HPP
#define P0RT_TO_CPP_HPP


#include "p0run/string.hpp"
#include <map>


namespace p0
{
	namespace rt
	{
		inline void to_cpp(run::value &to,
		                   run::value from)
		{
			to = from;
		}

		template <class Integer,
		          class IsCompatible = typename std::enable_if<boost::is_integral<Integer>::value, void>::type>
		void to_cpp(Integer &to,
		            run::value from)
		{
			to = static_cast<Integer>(run::to_integer(from));
		}

		inline void to_cpp(std::string &to,
		                   run::value from)
		{
			to = run::expect_string(from);
		}

		template <class Key, class Value, class Compare, class Alloc>
		void to_cpp(std::map<Key, Value, Compare, Alloc> &to,
		            run::value from);
		//TODO implement
	}
}


#endif

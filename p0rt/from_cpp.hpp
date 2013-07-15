#pragma once
#ifndef P0RT_FROM_CPP_HPP
#define P0RT_FROM_CPP_HPP


#include "p0run/string.hpp"
#include "p0run/table.hpp"
#include "p0run/construct.hpp"
#include "p0run/interpreter.hpp"
#include <map>
#include <boost/foreach.hpp>


namespace p0
{
	namespace rt
	{
		inline run::value from_cpp(run::value value, run::interpreter &)
		{
			return value;
		}

		template <class Integer,
		          class IsCompatible = typename std::enable_if<boost::is_integral<Integer>::value, void>::type>
		run::value from_cpp(Integer value,
		                           run::interpreter &)
		{
			return run::value(static_cast<run::integer>(value));
		}

		inline run::value from_cpp(std::string value,
		                           run::interpreter &interpreter)
		{
			auto &gc = interpreter.garbage_collector();
			return run::value(run::construct<run::string>(gc, std::move(value)));
		}

		template <class Key, class Value, class Compare, class Alloc>
		run::value from_cpp(std::map<Key, Value, Compare, Alloc> const &map,
		                    run::interpreter &interpreter)
		{
			run::table::elements elements;
			BOOST_FOREACH (auto const &entry, map)
			{
				auto const key = from_cpp(entry.first, interpreter);
				auto const value = from_cpp(entry.second, interpreter);
				elements.insert(std::make_pair(key, value));
			}
			auto &gc = interpreter.garbage_collector();
			return run::value(run::construct<run::table>(gc, std::move(elements)));
		}
	}
}


#endif

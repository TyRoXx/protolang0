#pragma once
#ifndef P0RUN_STRING_LITERAL_CACHE_HPP
#define P0RUN_STRING_LITERAL_CACHE_HPP


#include "value.hpp"
#include "p0common/final.hpp"
#include <unordered_map>
#include <memory>
#include <boost/functional/hash.hpp>


namespace p0
{
	namespace intermediate
	{
		struct unit;
	}

	namespace run
	{
		struct string_literal_cache PROTOLANG0_FINAL_CLASS
		{
			string_literal_cache();
			~string_literal_cache();
			value require_string(intermediate::unit const &program,
								 std::size_t string_index);

		private:

			typedef std::pair<intermediate::unit const *, std::size_t> string_key;
			typedef std::unordered_map<string_key, std::unique_ptr<object>, boost::hash<string_key>>
				strings_by_unit;

			strings_by_unit m_strings;
		};
	}
}


#endif

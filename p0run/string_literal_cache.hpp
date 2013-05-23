#pragma once
#ifndef P0RUN_STRING_LITERAL_CACHE_HPP
#define P0RUN_STRING_LITERAL_CACHE_HPP


#include "value.hpp"
#include "p0common/final.hpp"
#include <unordered_map>
#include <vector>
#include <memory>


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

			typedef std::vector<std::unique_ptr<object>> unit_strings;
			typedef std::unordered_map<intermediate::unit const *, unit_strings>
				strings_by_unit;

			strings_by_unit m_strings;
		};
	}
}


#endif

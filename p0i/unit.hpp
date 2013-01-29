#pragma once
#ifndef P0I_UNIT_HPP
#define P0I_UNIT_HPP


#include "function.hpp"
#include <vector>
#include <string>


namespace p0
{
	namespace intermediate
	{
		struct unit
		{
			typedef std::vector<function> function_vector;
			typedef std::vector<std::string> string_vector;


			explicit unit(
				function_vector functions,
				string_vector strings
				);
			function_vector const &functions() const;
			string_vector const &strings() const;

		private:

			function_vector m_functions;
			string_vector m_strings;
		};
	}
}


#endif

#pragma once
#ifndef P0I_UNIT_HPP
#define P0I_UNIT_HPP


#include "function.hpp"
#include "unit_info.hpp"
#include <vector>
#include <string>
#include <memory>


namespace p0
{
	namespace intermediate
	{
		struct unit PROTOLANG0_FINAL_CLASS
		{
			typedef std::vector<function> function_vector;
			typedef std::vector<std::string> string_vector;


			explicit unit(
				function_vector functions,
				string_vector strings,
				std::unique_ptr<unit_info> info = nullptr
				);
			function_vector const &functions() const;
			string_vector const &strings() const;
			unit_info const *info() const;

		private:

			function_vector m_functions;
			string_vector m_strings;
			std::unique_ptr<unit_info> m_info;
		};
	}
}


#endif

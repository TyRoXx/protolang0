#pragma once
#ifndef P0I_UNIT_INFO_HPP
#define P0I_UNIT_INFO_HPP


#include <string>
#include <vector>


namespace p0
{
	namespace intermediate
	{
		struct instruction_position
		{
			std::size_t row, column;
		};


		struct function_info
		{
			std::string name;
			std::vector<instruction_position> instructions;
		};


		struct unit_info
		{
			std::string name;
			std::vector<function_info> functions;
		};
	}
}


#endif

#pragma once
#ifndef P0I_FUNCTION_HPP
#define P0I_FUNCTION_HPP


#include "instruction.hpp"
#include <vector>
#include <cstddef>


namespace p0
{
	namespace intermediate
	{
		struct function
		{
			typedef std::vector<instruction> instruction_vector;


			function();
			explicit function(
				instruction_vector body,
				size_t parameters
				);
			instruction_vector const &body() const;
			size_t parameters() const;

		private:

			instruction_vector m_body;
			size_t m_parameters;
		};
	}
}


#endif

#pragma once
#ifndef P0C_CODE_GENERATOR_HPP
#define P0C_CODE_GENERATOR_HPP


#include "p0i/unit.hpp"
#include <functional>


namespace p0
{
	struct unit_generator;
	struct compiler_error;
	struct function_tree;


	struct code_generator
	{
		typedef std::function<bool (compiler_error const &)> compiler_error_handler;


		explicit code_generator(
			unit_generator &unit
			);
		unit_generator &unit() const;
		size_t generate_function(
			function_tree const &function
			);
		void handle_error(
			compiler_error const &error
			);
		void add_return(std::size_t jump_address);

	private:

		unit_generator &m_unit;
		std::vector<std::size_t> m_return_instructions;
	};
}


#endif

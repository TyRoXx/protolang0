#pragma once
#ifndef P0C_UNIT_GENERATOR_HPP
#define P0C_UNIT_GENERATOR_HPP


#include "p0i/unit.hpp"
#include <functional>
#include <unordered_map>


namespace p0
{
	struct compiler_error;
	struct function_tree;


	struct unit_generator
	{
		typedef std::function<bool (compiler_error const &)> compiler_error_handler;


		explicit unit_generator(
			function_tree const &tree,
			compiler_error_handler error_handler
			);
		function_tree const &tree() const;
		compiler_error_handler const &error_handler() const;
		intermediate::unit generate_unit();
		void handle_error(
			compiler_error const &error
			);
		size_t get_string_id(
			std::string value
			);
		std::size_t allocate_function();
		void define_function(std::size_t id, intermediate::function definition);

	private:

		typedef std::unordered_map<std::string, size_t> string_id_table;


		function_tree const &m_tree;
		compiler_error_handler const m_error_handler;
		intermediate::unit::function_vector m_functions;
		string_id_table m_string_ids;
	};
}


#endif

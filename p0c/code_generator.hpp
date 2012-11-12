#pragma once
#ifndef P0C_CODE_GENERATOR_HPP
#define P0C_CODE_GENERATOR_HPP


#include "p0i/unit.hpp"
#include <functional>
#include <unordered_map>


namespace p0
{
	struct statement_tree;
	struct expression_tree;
	struct function_tree;
	struct local_frame;
	struct compiler_error;

	namespace intermediate
	{
		struct emitter;
	}


	struct code_generator
	{
		typedef std::function<bool (compiler_error const &)> compiler_error_handler;


		explicit code_generator(
			function_tree const &tree,
			size_t integer_width,
			compiler_error_handler error_handler
			);
		size_t integer_width() const;
		intermediate::unit generate_unit();
		size_t generate_function(
			function_tree const &function
			);
		void handle_error(
			compiler_error const &error
			);
		size_t get_string_id(
			std::string value
			);

	private:

		typedef std::unordered_map<std::string, size_t> string_id_table;


		function_tree const &m_tree;
		size_t const m_integer_width;
		compiler_error_handler const m_error_handler;
		intermediate::unit::function_vector m_functions;
		string_id_table m_string_ids;
	};
}


#endif

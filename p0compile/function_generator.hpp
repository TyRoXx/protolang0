#pragma once
#ifndef P0C_FUNCTION_GENERATOR_HPP
#define P0C_FUNCTION_GENERATOR_HPP


#include "p0i/unit.hpp"
#include "reference.hpp"
#include <functional>


namespace p0
{
	struct unit_generator;
	struct compiler_error;
	struct function_tree;
	struct local_frame;

	namespace intermediate
	{
		struct emitter;
	}


	struct function_generator PROTOLANG0_FINAL_CLASS
	{
		typedef std::function<bool (compiler_error const &)> compiler_error_handler;


		explicit function_generator(
			unit_generator &unit
			);
		explicit function_generator(
			function_generator &parent,
			local_frame *outer_frame
			);
		unit_generator &unit() const;
		local_frame *outer_frame() const;
		void emit_function(
			function_tree const &function,
			reference destination
			);
		void handle_error(
			compiler_error const &error
			);
		void add_return(std::size_t jump_address);
		std::size_t bind(reference bound_variable);

	private:

		unit_generator &m_unit;
		local_frame * const m_outer_frame;
		function_generator * const m_parent;
		std::vector<std::size_t> m_return_instructions;
		std::vector<reference> m_bound_variables;


		void emit_bindings(
			size_t closure_address,
			intermediate::emitter &emitter) const;
	};
}


#endif

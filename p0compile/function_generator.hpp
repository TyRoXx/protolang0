#pragma once
#ifndef P0C_FUNCTION_GENERATOR_HPP
#define P0C_FUNCTION_GENERATOR_HPP


#include "p0i/unit.hpp"
#include "reference.hpp"
#include <functional>
#include <boost/variant.hpp>


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
		size_t generate_function(
			function_tree const &function
			);
		void handle_error(
			compiler_error const &error
			);
		void add_return(std::size_t jump_address);
		std::size_t bind_local(reference bound_variable);
		std::size_t bind_from_parent(size_t index_in_parent);
		void emit_bindings(
			size_t closure_address,
			local_frame &frame,
			intermediate::emitter &emitter) const;

	private:

		typedef size_t bound_from_parent;
		typedef reference bound_locally;

		typedef boost::variant<bound_from_parent, bound_locally>
			bound_variable;

		struct bound_variable_emitter;


		unit_generator &m_unit;
		local_frame * const m_outer_frame;
		std::vector<std::size_t> m_return_instructions;
		std::vector<bound_variable> m_bound_variables;


		std::size_t bind(bound_variable const &bound);
	};
}


#endif

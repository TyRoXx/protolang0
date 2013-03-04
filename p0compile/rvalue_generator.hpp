#pragma once
#ifndef P0C_RVALUE_GENERATOR_HPP
#define P0C_RVALUE_GENERATOR_HPP


#include "expression_tree.hpp"
#include "reference.hpp"
#include "p0i/emitter.hpp"
#include "p0common/final.hpp"
#include <functional>


namespace p0
{
	struct local_frame;
	struct function_generator;


	struct rvalue_generator PROTOLANG0_FINAL_CLASS : expression_tree_visitor
	{
		///@destination may be invalid to indicate that the value is not supposed to be stored
		explicit rvalue_generator(
			function_generator &function_generator,
			intermediate::emitter &emitter,
			local_frame &frame,
			reference destination
			);

	private:

		function_generator &m_function_generator;
		intermediate::emitter &m_emitter;
		local_frame &m_frame;
		reference const m_destination;


		void with_arguments(
				std::vector<std::unique_ptr<expression_tree>> const &arguments,
				std::function<void ()> const &handle_result
				);
		virtual void visit(name_expression_tree const &expression) PROTOLANG0_FINAL_METHOD;
		virtual void visit(integer_10_expression_tree const &expression) PROTOLANG0_FINAL_METHOD;
		virtual void visit(string_expression_tree const &expression) PROTOLANG0_FINAL_METHOD;
		virtual void visit(call_expression_tree const &expression) PROTOLANG0_FINAL_METHOD;
		virtual void visit(function_tree const &expression) PROTOLANG0_FINAL_METHOD;
		virtual void visit(null_expression_tree const &expression) PROTOLANG0_FINAL_METHOD;
		virtual void visit(table_expression_tree const &expression) PROTOLANG0_FINAL_METHOD;
		virtual void visit(unary_expression_tree const &expression) PROTOLANG0_FINAL_METHOD;
		virtual void visit(binary_expression_tree const &expression) PROTOLANG0_FINAL_METHOD;
		virtual void visit(dot_element_expression_tree const &expression) PROTOLANG0_FINAL_METHOD;
		virtual void visit(subscript_expression_tree const &expression) PROTOLANG0_FINAL_METHOD;
		virtual void visit(import_expression_tree const &expression) PROTOLANG0_FINAL_METHOD;
		virtual void visit(method_call_expression_tree const &expression) PROTOLANG0_FINAL_METHOD;
	};
}


#endif

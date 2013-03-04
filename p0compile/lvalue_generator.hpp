#pragma once
#ifndef P0C_LVALUE_GENERATOR_HPP
#define P0C_LVALUE_GENERATOR_HPP


#include "expression_tree.hpp"
#include "reference.hpp"
#include "p0i/emitter.hpp"
#include "p0common/final.hpp"
#include <functional>


namespace p0
{
	struct local_frame;
	struct function_generator;


	struct lvalue_generator PROTOLANG0_FINAL_CLASS : expression_tree_visitor
	{
		explicit lvalue_generator(
			function_generator &function_generator,
			intermediate::emitter &emitter,
			local_frame &frame
			);
		reference address() const;
		void commit_write();

	private:

		function_generator &m_function_generator;
		intermediate::emitter &m_emitter;
		local_frame &m_frame;
		reference m_address;
		std::function<void ()> m_commit_write;


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

#pragma once
#ifndef P0C_LVALUE_GENERATOR_HPP
#define P0C_LVALUE_GENERATOR_HPP


#include "expression_tree.hpp"
#include "reference.hpp"
#include "p0i/emitter.hpp"
#include <functional>


namespace p0
{
	struct local_frame;
	struct code_generator;


	struct lvalue_generator : expression_tree_visitor
	{
		explicit lvalue_generator(
			code_generator &function_generator,
			intermediate::emitter &emitter,
			local_frame &frame
			);
		reference address() const;
		void commit_write();

	private:

		code_generator &m_function_generator;
		intermediate::emitter &m_emitter;
		local_frame &m_frame;
		reference m_address;
		std::function<void ()> m_commit_write;


		virtual void visit(name_expression_tree const &expression) override;
		virtual void visit(integer_10_expression_tree const &expression) override;
		virtual void visit(string_expression_tree const &expression) override;
		virtual void visit(call_expression_tree const &expression) override;
		virtual void visit(function_tree const &expression) override;
		virtual void visit(null_expression_tree const &expression) override;
		virtual void visit(table_expression_tree const &expression) override;
		virtual void visit(unary_expression_tree const &expression) override;
		virtual void visit(binary_expression_tree const &expression) override;
		virtual void visit(dot_element_expression_tree const &expression) override;
	};
}


#endif

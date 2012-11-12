#pragma once
#ifndef P0C_STATEMENT_CODE_GENERATOR_HPP
#define P0C_STATEMENT_CODE_GENERATOR_HPP


#include "statement_tree.hpp"
#include "p0i/emitter.hpp"


namespace p0
{
	struct local_frame;
	struct code_generator;


	struct statement_code_generator : statement_tree_visitor
	{
		explicit statement_code_generator(
			code_generator &function_generator,
			intermediate::emitter &emitter,
			local_frame &frame
			);

	private:

		code_generator &m_function_generator;
		intermediate::emitter &m_emitter;
		local_frame &m_frame;


		virtual void visit(declaration_tree const &statement) override;
		virtual void visit(return_tree const &statement) override;
		virtual void visit(block_tree const &statement) override;
		virtual void visit(expression_statement_tree const &statement) override;
		virtual void visit(assignment_tree const &statement) override;
		virtual void visit(if_tree const &statement) override;
		virtual void visit(while_tree const &statement) override;
		virtual void visit(break_tree const &statement) override;
		virtual void visit(continue_tree const &statement) override;
	};


	void generate_statement(
		statement_tree const &tree,
		code_generator &function_generator,
		intermediate::emitter &emitter,
		local_frame &frame
		);
}


#endif

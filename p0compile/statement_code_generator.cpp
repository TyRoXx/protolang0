#include "statement_code_generator.hpp"
#include "local_frame.hpp"
#include "lvalue_generator.hpp"
#include "rvalue_generator.hpp"
#include "code_generator.hpp"
#include "compiler_error.hpp"
#include "temporary.hpp"


namespace p0
{
	statement_code_generator::statement_code_generator(
		code_generator &function_generator,
		intermediate::emitter &emitter,
		local_frame &frame
		)
		: m_function_generator(function_generator)
		, m_emitter(emitter)
		, m_frame(frame)
	{
	}

	void statement_code_generator::visit(declaration_tree const &statement)
	{
		auto const address = m_frame.declare_variable(statement.name());

		rvalue_generator source(
			m_function_generator,
			m_emitter,
			m_frame,
			address
			);
		statement.value().accept(source);
	}

	void statement_code_generator::visit(return_tree const &statement)
	{
		//TODO: calling convention
		temporary const return_value(
			m_frame,
			1);

		{
			rvalue_generator return_value_generator(
				m_function_generator,
				m_emitter,
				m_frame,
				return_value.address()
				);
			statement.value().accept(return_value_generator);
		}

		m_emitter.return_();
	}

	void statement_code_generator::visit(block_tree const &statement)
	{
		local_frame block_symbols(&m_frame);

		for (auto s = begin(statement.body()); s != end(statement.body()); ++s)
		{
			try
			{
				generate_statement(
					**s,
					m_function_generator,
					m_emitter,
					block_symbols
					);
			}
			catch (compiler_error const &e)
			{
				m_function_generator.handle_error(e);
			}
		}
	}

	void statement_code_generator::visit(expression_statement_tree const &statement)
	{
		rvalue_generator value(
			m_function_generator,
			m_emitter,
			m_frame,
			reference() //discard result
			);
		statement.expression().accept(value);
	}

	void statement_code_generator::visit(assignment_tree const &statement)
	{
		lvalue_generator destination(
			m_function_generator,
			m_emitter,
			m_frame
			);
		statement.destination().accept(destination);

		assert(destination.address().is_valid());
		
		rvalue_generator source(
			m_function_generator,
			m_emitter,
			m_frame,
			destination.address()
			);
		statement.source().accept(source);

		destination.commit_write();
	}

	void statement_code_generator::visit(if_tree const &statement)
	{
		//TODO: only create a variable if needed
		temporary const condition_variable(
			m_frame,
			1
			);

		{
			rvalue_generator condition(
				m_function_generator,
				m_emitter,
				m_frame,
				condition_variable.address()
				);
			statement.condition().accept(condition);
		}

		m_emitter.not_(
			condition_variable.address().local_address()
			);

		auto const jump_if_address = m_emitter.get_current_jump_address();
		m_emitter.jump_if(
			0,
			condition_variable.address().local_address()
			);

		generate_statement(
			statement.on_true(),
			m_function_generator,
			m_emitter,
			m_frame
			);

		auto const * const on_false = statement.on_false();

		auto const skip_else_address = m_emitter.get_current_jump_address();
		if (on_false)
		{
			m_emitter.jump(
				0
				);
		}

		m_emitter.update_jump_destination(
			jump_if_address,
			m_emitter.get_current_jump_address()
			);

		if (on_false)
		{
			generate_statement(
				*on_false,
				m_function_generator,
				m_emitter,
				m_frame
				);

			m_emitter.update_jump_destination(
				skip_else_address,
				m_emitter.get_current_jump_address()
				);
		}
	}

	void statement_code_generator::visit(while_tree const &statement)
	{
		temporary condition_variable(
			m_frame,
			1
			);

		const auto at_condition_check = m_emitter.get_current_jump_address();

		{
			rvalue_generator condition(
				m_function_generator,
				m_emitter,
				m_frame,
				condition_variable.address()
				);
			statement.condition().accept(condition);
		}

		m_emitter.not_(condition_variable.address().local_address());

		const auto at_skip_body = m_emitter.get_current_jump_address();
		m_emitter.jump_if(
			0,
			condition_variable.address().local_address()
			);

		generate_statement(
			statement.body(),
			m_function_generator,
			m_emitter,
			m_frame
			);

		m_emitter.jump(at_condition_check);

		const auto at_after_loop = m_emitter.get_current_jump_address();
		m_emitter.update_jump_destination(
			at_skip_body,
			at_after_loop);
	}

	void statement_code_generator::visit(break_tree const &statement)
	{
	}

	void statement_code_generator::visit(continue_tree const &statement)
	{
	}


	void generate_statement(
		statement_tree const &tree,
		code_generator &function_generator,
		intermediate::emitter &emitter,
		local_frame &frame
		)
	{
		statement_code_generator generator(
			function_generator,
			emitter,
			frame
			);
		tree.accept(generator);
	}
}

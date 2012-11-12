#include "lvalue_generator.hpp"
#include "local_frame.hpp"
#include "compiler_error.hpp"
#include "temporary.hpp"
#include "rvalue_generator.hpp"
#include "code_generator.hpp"


namespace p0
{
	lvalue_generator::lvalue_generator(
		code_generator &function_generator,
		intermediate::emitter &emitter,
		local_frame &frame
		)
		: m_function_generator(function_generator)
		, m_emitter(emitter)
		, m_frame(frame)
	{
	}

	reference lvalue_generator::address() const
	{
		return m_address;
	}

	void lvalue_generator::commit_write()
	{
		if (m_commit_write)
		{
			m_commit_write();
		}
	}


	void lvalue_generator::visit(name_expression_tree const &expression)
	{
		auto const address = m_frame.require_symbol(
			expression.name()
			);

		m_address = address;
	}

	void lvalue_generator::visit(integer_10_expression_tree const &expression)
	{
		throw compiler_error(
			"An integer literal is not an LValue",
			expression.position()
			);
	}

	void lvalue_generator::visit(string_expression_tree const &expression)
	{
		throw compiler_error(
			"A string literal is not an LValue",
			expression.position()
			);
	}

	void lvalue_generator::visit(call_expression_tree const &expression)
	{
		throw compiler_error(
			"A function result is not an LValue",
			expression.position()
			);
	}

	void lvalue_generator::visit(function_tree const &expression)
	{
		throw compiler_error(
			"A function is not an LValue",
			expression.position()
			);
	}

	void lvalue_generator::visit(null_expression_tree const &expression)
	{
		throw compiler_error(
			"'null' is not an LValue",
			expression.position()
			);
	}

	void lvalue_generator::visit(table_expression_tree const &expression)
	{
		throw compiler_error(
			"A table literal is not an LValue",
			expression.position()
			);
	}

	void lvalue_generator::visit(unary_expression_tree const &expression)
	{
		throw compiler_error(
			"A unary operator expression is not an LValue",
			expression.position()
			);
	}

	void lvalue_generator::visit(binary_expression_tree const &expression)
	{
		throw compiler_error(
			"A binary operator expression is not an LValue",
			expression.position()
			);
	}

	void lvalue_generator::visit(dot_element_expression_tree const &expression)
	{
		auto const element_variable = std::make_shared<temporary>(
			std::ref(m_frame),
			1
			);

		m_address = element_variable->address();

		auto &table_expression = expression.table();
		auto const element_name = expression.element_name();

		m_commit_write = [this, element_variable, &table_expression, element_name]()
		{
			temporary const table_variable(
				std::ref(m_frame),
				1
				);

			rvalue_generator table_generator(
				m_function_generator,
				m_emitter,
				m_frame,
				table_variable.address()
				);
			table_expression.accept(table_generator);

			temporary const key_variable(
				std::ref(m_frame),
				1
				);

			auto key = source_range_to_string(element_name);
			auto const key_string_id = m_function_generator.get_string_id(
				std::move(key)
				);

			m_emitter.set_string(
				key_variable.address().local_address(),
				key_string_id
				);

			m_emitter.set_element(
				table_variable.address().local_address(),
				key_variable.address().local_address(),
				element_variable->address().local_address()
				);
		};
	}
}

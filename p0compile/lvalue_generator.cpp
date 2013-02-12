#include "lvalue_generator.hpp"
#include "local_frame.hpp"
#include "compiler_error.hpp"
#include "temporary.hpp"
#include "rvalue_generator.hpp"
#include "code_generator.hpp"
#include "unit_generator.hpp"


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

	namespace
	{
		void generate_subscript(
			local_frame &frame,
			intermediate::emitter &emitter,
			code_generator &function_generator,
			reference &destination,
			std::function<void ()> &commit_write,
			expression_tree const &table_expression,
			std::function<void (reference &key_destination)> const &emit_key
			)
		{
			auto const element_variable = std::make_shared<temporary>(
				std::ref(frame),
				1
				);

			destination = element_variable->address();

			commit_write =
				[&frame, &function_generator, &emitter,
				element_variable, &table_expression, emit_key]()
			{
				temporary const table_variable(
					std::ref(frame),
					1
					);

				rvalue_generator table_generator(
					function_generator,
					emitter,
					frame,
					table_variable.address()
					);
				table_expression.accept(table_generator);

				temporary const key_variable(
					std::ref(frame),
					1
					);

				reference key_address = key_variable.address();
				emit_key(
					key_address
					);

				emitter.set_element(
					table_variable.address().local_address(),
					key_address.local_address(),
					element_variable->address().local_address()
					);
			};
		}
	}

	void lvalue_generator::visit(dot_element_expression_tree const &expression)
	{
		auto const element_name = expression.element_name();
		auto &function_generator = m_function_generator;
		auto &emitter = m_emitter;

		generate_subscript(
			m_frame,
			m_emitter,
			m_function_generator,
			m_address,
			m_commit_write,
			expression.table(),
			[element_name, &function_generator, &emitter](reference &key_destination)
		{
			auto key = source_range_to_string(element_name);
			auto const key_string_id = function_generator.unit().get_string_id(
				std::move(key)
				);

			emitter.set_string(
				key_destination.local_address(),
				key_string_id
				);
		});
	}

	void lvalue_generator::visit(subscript_expression_tree const &expression)
	{
		auto const &key_expression = expression.key();
		auto &function_generator = m_function_generator;
		auto &emitter = m_emitter;
		auto &frame = m_frame;

		generate_subscript(
			m_frame,
			m_emitter,
			m_function_generator,
			m_address,
			m_commit_write,
			expression.table(),
			[&key_expression, &function_generator, &emitter, &frame](reference &key_destination)
		{
			rvalue_generator key_generator(
				function_generator,
				emitter,
				frame,
				key_destination
				);
			key_expression.accept(key_generator);
		});
	}

	void lvalue_generator::visit(load_module_expression_tree const &expression)
	{
		throw compiler_error(
			"A load_module expression is not an LValue",
			expression.position()
			);
	}
}

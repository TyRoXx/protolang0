#include "p0compile/parser.hpp"
#include "p0compile/expression_tree.hpp"
#include "p0compile/statement_tree.hpp"
#include "p0compile/scanner.hpp"
#include "p0compile/compiler_error.hpp"
#include <boost/test/unit_test.hpp>


namespace
{
	template <class ASTHandler, class ErrorHandler>
	void check_source(std::string const &source,
					  ASTHandler const &handle_ast,
					  ErrorHandler const &handle_error)
	{
		p0::source_range const source_range(
					source.data(),
					source.data() + source.size());
		p0::scanner scanner(source_range);
		p0::parser parser(scanner, handle_error);
		auto const ast = parser.parse_unit();
		BOOST_REQUIRE(ast);
		handle_ast(*ast);
	}

	bool handle_unexpected_error(p0::compiler_error const &error)
	{
		(void)error;
		BOOST_REQUIRE(nullptr == "No error expected");
		return true;
	}

	template <class ASTHandler>
	void check_valid_source(std::string const &source,
							ASTHandler const &handle_ast)
	{
		return check_source(source, handle_ast, handle_unexpected_error);
	}

	template <class Base>
	struct down_caster
	{
		down_caster(Base const &base)
			: m_base(&base)
		{
		}

		template <class Down>
		operator Down const &() const
		{
			assert(m_base);
			return dynamic_cast<Down const &>(*m_base);
		}

	private:

		Base const *m_base;
	};

	template <class Handler>
	void check_expression(p0::expression_tree const &expression, Handler const &handler)
	{
		handler(down_caster<p0::expression_tree>(expression));
	}

	template <class Handler>
	void check_statement(p0::statement_tree const &statement, Handler const &handler)
	{
		handler(down_caster<p0::statement_tree>(statement));
	}

	bool test_syntax_error(std::string const &correct_head,
						   std::string const &illformed_tail)
	{
		auto const full_source = correct_head + illformed_tail;
		auto const error_position = correct_head.size();
		bool found_expected_error = false;

		auto const check_compiler_error =
			[&full_source, error_position, &found_expected_error]
			(p0::compiler_error const &error) -> bool
		{
			auto const found_error_position =
				static_cast<size_t>(error.position().begin() - full_source.data());
			found_expected_error = (error_position == found_error_position);
			return true;
		};

		p0::source_range const source_range(
			full_source.data(),
			full_source.data() + full_source.size());
		p0::scanner scanner(source_range);
		p0::parser parser(scanner, check_compiler_error);
		parser.parse_unit();
		return found_expected_error;
	}
}

BOOST_AUTO_TEST_CASE(parse_empty_file_test)
{
	check_valid_source("", [](p0::function_tree const &ast)
	{
		BOOST_CHECK(ast.parameters().empty());
		check_statement(ast.body(), [](p0::block_tree const &block)
		{
			BOOST_CHECK(block.body().empty());
		});
	});
}

BOOST_AUTO_TEST_CASE(parse_return_test)
{
	check_valid_source("return null", [](p0::function_tree const &ast)
	{
		BOOST_CHECK(ast.parameters().empty());
		check_statement(ast.body(), [](p0::block_tree const &block)
		{
			BOOST_REQUIRE(block.body().size() == 1);
			check_statement(*block.body()[0], [](p0::return_tree const &return_)
			{
				check_expression(return_.value(), [](p0::null_expression_tree const &)
				{
				});
			});
		});
	});
}

BOOST_AUTO_TEST_CASE(parse_load_module_test)
{
	check_valid_source("import name", [](p0::function_tree const &ast)
	{
		BOOST_CHECK(ast.parameters().empty());
		check_statement(ast.body(), [](p0::block_tree const &block)
		{
			BOOST_REQUIRE(block.body().size() == 1);
			check_statement(*block.body()[0],
				[](p0::expression_statement_tree const &statement)
			{
				check_expression(statement.expression(),
					[](p0::import_expression_tree const &load_module)
				{
					check_expression(load_module.name(),
						[](p0::name_expression_tree const &name)
					{
						BOOST_CHECK("name" == p0::source_range_to_string(name.name()));
					});
				});
			});
		});
	});
}

BOOST_AUTO_TEST_CASE(invalid_table_literal_test)
{
	//a table is not an array
	BOOST_CHECK(test_syntax_error("[", "1, 2, 3]"));

	//an extra comma is not permitted
	BOOST_CHECK(test_syntax_error("[a = 1,", ",]"));

	//a comma alone is not permitted
	BOOST_CHECK(test_syntax_error("[", ",]"));

	//a keyword cannot be used as a table key
	BOOST_CHECK(test_syntax_error("[", "null = 1]"));
}

BOOST_AUTO_TEST_CASE(missing_expression_test)
{
	BOOST_CHECK(test_syntax_error("return ", ""));
	BOOST_CHECK(test_syntax_error("import ", ""));
	BOOST_CHECK(test_syntax_error("5 + ", ""));
	BOOST_CHECK(test_syntax_error("f((((", "***"));
}

BOOST_AUTO_TEST_CASE(outmost_scope_closing_brace_test)
{
	BOOST_CHECK(test_syntax_error("", "}"));
}

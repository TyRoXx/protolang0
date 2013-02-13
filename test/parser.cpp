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

	void check_invalid_source(std::string const &source,
							  std::size_t error_position)
	{
		bool has_failed = false;
		check_source(
			source,
			[](p0::function_tree const &)
		{
			//ignore results
		},
			[error_position, &source, &has_failed](p0::compiler_error const &error) -> bool
		{
			auto const error_found_at = error.position().begin();
			BOOST_CHECK(source.data() + error_position == error_found_at);
			has_failed = true;
			return true;
		});
		BOOST_CHECK(has_failed);
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
			check_statement(*block.body()[0], [](p0::expression_statement_tree const &statement)
			{
				check_expression(statement.expression(), [](p0::import_expression_tree const &load_module)
				{
					check_expression(load_module.name(), [](p0::name_expression_tree const &name)
					{
						BOOST_CHECK("name" == p0::source_range_to_string(name.name()));
					});
				});
			});
		});
	});
}

BOOST_AUTO_TEST_CASE(missing_expression_test)
{
	check_invalid_source("return ", 7);
	check_invalid_source("import ", 7);
	check_invalid_source("5 + ", 4);
	check_invalid_source("f((((***", 5);
}

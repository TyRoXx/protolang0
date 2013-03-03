#include "p0compile/parser.hpp"
#include "p0compile/expression_tree.hpp"
#include "p0compile/statement_tree.hpp"
#include "p0compile/scanner.hpp"
#include "p0compile/compiler_error.hpp"
#include <boost/test/unit_test.hpp>


namespace
{
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

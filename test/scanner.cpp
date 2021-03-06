#include "p0compile/scanner.hpp"
#include "p0compile/compiler_error.hpp"
#include <boost/test/unit_test.hpp>


namespace
{
	template <class ScannerHandler>
	void scan(
		std::string const &code,
		ScannerHandler const &handler
		)
	{
		p0::source_range const code_range(code.data(), code.data() + code.size());
		p0::scanner scanner(code_range);
		handler(scanner);
	}
}

BOOST_AUTO_TEST_CASE(empty_source_test)
{
	scan("", [](p0::scanner &scanner)
	{
		BOOST_CHECK(scanner.next_token().type == p0::token_type::end_of_file);
	});

	scan("    \n\n\t\r\n    ", [](p0::scanner &scanner)
	{
		BOOST_CHECK(scanner.next_token().type == p0::token_type::end_of_file);
	});

	scan("; comment", [](p0::scanner &scanner)
	{
		BOOST_CHECK(scanner.next_token().type == p0::token_type::end_of_file);
	});

	scan("; comment\n", [](p0::scanner &scanner)
	{
		BOOST_CHECK(scanner.next_token().type == p0::token_type::end_of_file);
	});
}

namespace
{
	bool scan_single_token(
		std::string const &code,
		p0::token_type::Enum type
		)
	{
		p0::source_range const code_range(code.data(), code.data() + code.size());
		p0::scanner scanner(code_range);

		return
			(scanner.next_token().type == type) &&
			(scanner.next_token().type == p0::token_type::end_of_file);
	}
}

BOOST_AUTO_TEST_CASE(operator_test)
{
	using namespace p0::token_type;

	BOOST_CHECK(scan_single_token("(", parenthesis_left));
	BOOST_CHECK(scan_single_token(")", parenthesis_right));
	BOOST_CHECK(scan_single_token("{", brace_left));
	BOOST_CHECK(scan_single_token("}", brace_right));
	BOOST_CHECK(scan_single_token("[", bracket_left));
	BOOST_CHECK(scan_single_token("]", bracket_right));
	BOOST_CHECK(scan_single_token("=", assign));
	BOOST_CHECK(scan_single_token(",", comma));
	BOOST_CHECK(scan_single_token("+", plus));
	BOOST_CHECK(scan_single_token("-", minus));
	BOOST_CHECK(scan_single_token("*", star));
	BOOST_CHECK(scan_single_token("/", slash));
	BOOST_CHECK(scan_single_token("%", modulo));
	BOOST_CHECK(scan_single_token("&", ampersand));
	BOOST_CHECK(scan_single_token("&&", ampersands));
	BOOST_CHECK(scan_single_token("|", p0::token_type::pipe));
	BOOST_CHECK(scan_single_token("||", pipes));
	BOOST_CHECK(scan_single_token("<<", shift_left));
	BOOST_CHECK(scan_single_token(">>", shift_right));
	BOOST_CHECK(scan_single_token(">>>", shift_signed));
	BOOST_CHECK(scan_single_token("<", less));
	BOOST_CHECK(scan_single_token("<=", less_equal));
	BOOST_CHECK(scan_single_token(">", greater));
	BOOST_CHECK(scan_single_token(">=", greater_equal));
	BOOST_CHECK(scan_single_token("==", equal));
	BOOST_CHECK(scan_single_token("!=", not_equal));
	BOOST_CHECK(scan_single_token("!", exclamation_mark));
	BOOST_CHECK(scan_single_token(".", dot));
	BOOST_CHECK(scan_single_token("~", tilde));
	BOOST_CHECK(scan_single_token("^", caret));
	BOOST_CHECK(scan_single_token(":", colon));
}

BOOST_AUTO_TEST_CASE(keyword_test)
{
	using namespace p0::token_type;

	BOOST_CHECK(scan_single_token("var", var));
	BOOST_CHECK(scan_single_token("function", function));
	BOOST_CHECK(scan_single_token("return", return_));
	BOOST_CHECK(scan_single_token("null", null));
	BOOST_CHECK(scan_single_token("if", if_));
	BOOST_CHECK(scan_single_token("else", else_));
	BOOST_CHECK(scan_single_token("while", while_));
	BOOST_CHECK(scan_single_token("break", break_));
	BOOST_CHECK(scan_single_token("continue", continue_));
	BOOST_CHECK(scan_single_token("import", import));
}

namespace
{
	bool is_identifier(
		std::string const &identifier,
		p0::token const &token
		)
	{
		return
			(token.type == p0::token_type::identifier) &&
			(p0::source_range_to_string(token.content) == identifier);
	}
}

BOOST_AUTO_TEST_CASE(identifier_test)
{
	scan("a bc def _u n0 _9", [](p0::scanner &scanner)
	{
		BOOST_CHECK(is_identifier("a", scanner.next_token()));
		BOOST_CHECK(is_identifier("bc", scanner.next_token()));
		BOOST_CHECK(is_identifier("def", scanner.next_token()));
		BOOST_CHECK(is_identifier("_u", scanner.next_token()));
		BOOST_CHECK(is_identifier("n0", scanner.next_token()));
		BOOST_CHECK(is_identifier("_9", scanner.next_token()));
	});

	{
		std::string const every_char =
			"abcdefghijklmnopqrstuvwxyz"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"0123456789"
			"_";

		scan(every_char, [&every_char](p0::scanner &scanner)
		{
			BOOST_CHECK(is_identifier(every_char, scanner.next_token()));
		});
	}
}

namespace
{
	bool is_integer(
		std::string const &value,
		p0::token const &token
		)
	{
		return
			(token.type == p0::token_type::integer_10) &&
			(p0::source_range_to_string(token.content) == value);
	}
}

BOOST_AUTO_TEST_CASE(integer_identifier_test)
{
	scan("123 abc", [](p0::scanner &scanner)
	{
		BOOST_CHECK(is_integer("123", scanner.next_token()));
		BOOST_CHECK(is_identifier("abc", scanner.next_token()));
	});

	scan("123\nabc", [](p0::scanner &scanner)
	{
		BOOST_CHECK(is_integer("123", scanner.next_token()));
		BOOST_CHECK(is_identifier("abc", scanner.next_token()));
	});

	scan("123;comment\nabc", [](p0::scanner &scanner)
	{
		BOOST_CHECK(is_integer("123", scanner.next_token()));
		BOOST_CHECK(is_identifier("abc", scanner.next_token()));
	});

	scan("123+abc", [](p0::scanner &scanner)
	{
		BOOST_CHECK(is_integer("123", scanner.next_token()));
		BOOST_CHECK(scanner.next_token().type == p0::token_type::plus);
		BOOST_CHECK(is_identifier("abc", scanner.next_token()));
	});

	scan("123 if", [](p0::scanner &scanner)
	{
		BOOST_CHECK(is_integer("123", scanner.next_token()));
		BOOST_CHECK(scanner.next_token().type == p0::token_type::if_);
	});
}

BOOST_AUTO_TEST_CASE(skip_line_test)
{
	scan("123 abc", [](p0::scanner &scanner)
	{
		scanner.skip_line();
		BOOST_CHECK(scanner.next_token().type == p0::token_type::end_of_file);
	});

	scan("123 abc", [](p0::scanner &scanner)
	{
		BOOST_CHECK(is_integer("123", scanner.next_token()));
		scanner.skip_line();
		BOOST_CHECK(scanner.next_token().type == p0::token_type::end_of_file);
	});

	scan("123\nabc", [](p0::scanner &scanner)
	{
		BOOST_CHECK(is_integer("123", scanner.next_token()));
		scanner.skip_line();
		BOOST_CHECK(is_identifier("abc", scanner.next_token()));
		BOOST_CHECK(scanner.next_token().type == p0::token_type::end_of_file);
	});

	scan("123 ?? def \nabc", [](p0::scanner &scanner)
	{
		BOOST_CHECK(is_integer("123", scanner.next_token()));
		scanner.skip_line();
		BOOST_CHECK(is_identifier("abc", scanner.next_token()));
		BOOST_CHECK(scanner.next_token().type == p0::token_type::end_of_file);
	});
}

namespace
{
	bool test_token_content(
		std::string const &source,
		size_t expected_begin_index,
		size_t expected_end_index)
	{
		p0::source_range source_range(
			source.data(),
			source.data() + source.size());
		p0::scanner scanner(source_range);
		auto const found_token = scanner.next_token();
		p0::source_range expected_content(
			source.data() + expected_begin_index,
			source.data() + expected_end_index);
		return (expected_content == found_token.content);
	}
}

BOOST_AUTO_TEST_CASE(token_content_test)
{
	BOOST_CHECK(test_token_content("( ", 0, 1));
	BOOST_CHECK(test_token_content(") ", 0, 1));
	BOOST_CHECK(test_token_content("} ", 0, 1));
	BOOST_CHECK(test_token_content("{ ", 0, 1));
	BOOST_CHECK(test_token_content("+ ", 0, 1));
	BOOST_CHECK(test_token_content(" ~ ", 1, 2));

	BOOST_CHECK(test_token_content("<= ", 0, 2));

	BOOST_CHECK(test_token_content("1234 56", 0, 4));
	BOOST_CHECK(test_token_content("1234", 0, 4));
	BOOST_CHECK(test_token_content(" 1234", 1, 5));

	BOOST_CHECK(test_token_content("identifier ", 0, 10));

	BOOST_CHECK(test_token_content("if ", 0, 2));
	BOOST_CHECK(test_token_content(" while ", 1, 6));
}

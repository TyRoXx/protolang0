#include "p0compile/compiler.hpp"
#include "p0compile/compiler_error.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/lexical_cast.hpp>


namespace
{
	bool test_invalid_source(std::string const &correct_head,
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
		p0::compiler compiler(source_range, check_compiler_error);
		compiler.compile(); //TODO make compile() throw
		return found_expected_error;
	}
}

BOOST_AUTO_TEST_CASE(misplaced_break_continue_test)
{
	//not allowed in outermost scope
	BOOST_CHECK(test_invalid_source("", "break"));
	BOOST_CHECK(test_invalid_source("", "continue"));

	//not allowed in a non-loop scope
	BOOST_CHECK(test_invalid_source("{", "break}"));
	BOOST_CHECK(test_invalid_source("{", "continue}"));

	//not allowed in the top-level scope of a function
	BOOST_CHECK(test_invalid_source("function () {", "break}"));

	//you cannot break out of a function
	BOOST_CHECK(test_invalid_source("while 1 { function () {", "break} }"));
}

BOOST_AUTO_TEST_CASE(overflowing_integer_literal_test)
{
	{
		auto const max_integer = std::numeric_limits<boost::int64_t>::max();
		auto const too_large = static_cast<boost::uint64_t>(max_integer) + 1;
		BOOST_CHECK(test_invalid_source("", boost::lexical_cast<std::string>(too_large)));
	}

	BOOST_CHECK(test_invalid_source("", std::string(500, '9')));
}

BOOST_AUTO_TEST_CASE(missing_expression_test)
{
	BOOST_CHECK(test_invalid_source("return ", ""));
	BOOST_CHECK(test_invalid_source("import ", ""));
	BOOST_CHECK(test_invalid_source("5 + ", ""));
	BOOST_CHECK(test_invalid_source("f((((", "***"));
}

BOOST_AUTO_TEST_CASE(outmost_scope_closing_brace_test)
{
	BOOST_CHECK(test_invalid_source("", "}"));
}

#include "p0compile/compiler.hpp"
#include "p0compile/compiler_error.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/lexical_cast.hpp>


namespace
{
	bool test_semantic_error(std::string const &correct_head,
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
	//break/continue are not allowed in outermost scope
	BOOST_CHECK(test_semantic_error("", "break"));
	BOOST_CHECK(test_semantic_error("", "continue"));

	//break/continue are not allowed in a non-loop scope
	BOOST_CHECK(test_semantic_error("{", "break}"));
	BOOST_CHECK(test_semantic_error("{", "continue}"));

	//break/continue are not allowed in the top-level scope of a function
	BOOST_CHECK(test_semantic_error("function () {", "break}"));

	//you cannot break/continue a loop outside of a function
	BOOST_CHECK(test_semantic_error("while 1 { function () {", "break} }"));
	BOOST_CHECK(test_semantic_error("while 1 { function () {", "continue} }"));
}

BOOST_AUTO_TEST_CASE(overflowing_integer_literal_test)
{
	{
		auto const max_integer = std::numeric_limits<boost::int64_t>::max();
		auto const too_large = static_cast<boost::uint64_t>(max_integer) + 1;
		BOOST_CHECK(test_semantic_error("", boost::lexical_cast<std::string>(too_large)));
	}

	BOOST_CHECK(test_semantic_error("", std::string(500, '9')));
}

BOOST_AUTO_TEST_CASE(immutable_bound_variable_test)
{
	//a variable is immutable when bound
	BOOST_CHECK(test_semantic_error(
		"var a = 2\n"
		"var f = function () { ", "a = a + 1 }\n"));
}

BOOST_AUTO_TEST_CASE(invalid_lvalue_test)
{
	BOOST_CHECK(test_semantic_error("", "null = null"));
	BOOST_CHECK(test_semantic_error("", "[] = null"));
	BOOST_CHECK(test_semantic_error("", "function () {} = null"));
	BOOST_CHECK(test_semantic_error("var f = function (){}\n", "f() = null"));
	BOOST_CHECK(test_semantic_error("\"", "\" = null")); //TODO fix position
	BOOST_CHECK(test_semantic_error("", "123 = null"));
	BOOST_CHECK(test_semantic_error("var a = null\n", "~a = null"));
	BOOST_CHECK(test_semantic_error("", "1 + 2 = null"));
	BOOST_CHECK(test_semantic_error("", "import \"\" = null"));
}

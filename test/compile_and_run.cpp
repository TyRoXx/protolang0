#include "p0run/interpreter.hpp"
#include "p0run/string.hpp"
#include "p0run/table.hpp"
#include "p0compile/compiler.hpp"
#include "p0compile/compiler_error.hpp"
#include <boost/test/unit_test.hpp>
using namespace p0::run;


namespace
{
	bool expect_no_error(p0::compiler_error const &error)
	{
		std::cerr << error.what() << '\n';
		BOOST_CHECK(nullptr == "no error expected");
		return true;
	}

	template <class CheckResult>
	void run_valid_source(
			std::string const &source,
			std::vector<value> const &arguments,
			CheckResult const &check_result)
	{
		p0::source_range const source_range(
			source.data(),
			source.data() + source.size());

		p0::compiler compiler(source_range, expect_no_error);
		auto const program = compiler.compile();

		p0::run::interpreter interpreter(program, nullptr);
		auto const &main_function = program.functions().front();
		auto const result = interpreter.call(main_function, arguments);
		check_result(result, program);
	}

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

BOOST_AUTO_TEST_CASE(empty_function_compile_test)
{
	std::string const source = "";
	std::vector<p0::run::value> const arguments;
	run_valid_source(source, arguments,
		[](p0::run::value const &result, p0::intermediate::unit const &program)
	{
		auto const &main_function = program.functions().front();
		BOOST_CHECK(result == value(main_function));
	});
}

BOOST_AUTO_TEST_CASE(return_integer_compile_test)
{
	std::string const source = "return 123";
	std::vector<value> const arguments;
	run_valid_source(source, arguments,
		[](value const &result, p0::intermediate::unit const & /*program*/)
	{
		BOOST_CHECK(result == value(static_cast<integer>(123)));
	});
}

BOOST_AUTO_TEST_CASE(return_null_compile_test)
{
	std::string const source = "return null";
	std::vector<value> const arguments;
	run_valid_source(source, arguments,
		[](value const &result, p0::intermediate::unit const & /*program*/)
	{
		BOOST_CHECK(is_null(result));
	});
}

BOOST_AUTO_TEST_CASE(return_string_compile_test)
{
	std::string const source = "return \"hello, world!\"";
	std::vector<value> const arguments;
	run_valid_source(source, arguments,
		[](value const &result, p0::intermediate::unit const & /*program*/)
	{
		BOOST_REQUIRE(result.type == value_type::object);
		BOOST_REQUIRE(dynamic_cast<string const *>(result.obj));
		BOOST_CHECK(dynamic_cast<string const &>(*result.obj).content() == "hello, world!");
	});
}

BOOST_AUTO_TEST_CASE(return_empty_table_compile_test)
{
	std::string const source = "return []";
	std::vector<value> const arguments;
	run_valid_source(source, arguments,
		[](value const &result, p0::intermediate::unit const & /*program*/)
	{
		BOOST_REQUIRE(result.type == value_type::object);
		BOOST_REQUIRE(dynamic_cast<table const *>(result.obj));
	});
}

BOOST_AUTO_TEST_CASE(return_trivial_table_compile_test)
{
	std::string const source = "var t = [] t[123] = 456 return t";
	std::vector<value> const arguments;
	run_valid_source(source, arguments,
		[](value const &result, p0::intermediate::unit const & /*program*/)
	{
		BOOST_REQUIRE(result.type == value_type::object);
		BOOST_REQUIRE(dynamic_cast<table const *>(result.obj));
		auto const element = dynamic_cast<table const &>(*result.obj).get_element(
					value(static_cast<integer>(123)));
		BOOST_REQUIRE(element);
		BOOST_CHECK(*element == value(static_cast<integer>(456)));
	});
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

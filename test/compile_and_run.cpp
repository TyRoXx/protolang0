#include "p0run/interpreter.hpp"
#include "p0run/string.hpp"
#include "p0run/table.hpp"
#include "p0compile/compiler.hpp"
#include <boost/test/unit_test.hpp>
using namespace p0::run;


namespace
{
	bool expect_no_error(p0::compiler_error const &error)
	{
		(void)error;
		BOOST_CHECK(!"no error expected");
		return true;
	}

	template <class CheckResult>
	void run_valid_source(
			std::string const &source,
			std::vector<value> const &arguments,
			CheckResult const &check_result)
	{
		p0::source_range const source_range(source.data(), source.data() + source.size());
		p0::compiler compiler(source_range, expect_no_error);
		auto const program = compiler.compile();

		p0::run::interpreter interpreter(program);
		auto const &main_function = program.functions().front();
		auto const result = interpreter.call(main_function, arguments);
		check_result(result, program);
	}
}

BOOST_AUTO_TEST_CASE(empty_function_compile_test)
{
	std::string const source = "{ }";
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
	std::string const source = "{ return 123 }";
	std::vector<value> const arguments;
	run_valid_source(source, arguments,
					 [](value const &result, p0::intermediate::unit const &program)
	{
		BOOST_CHECK(result == value(static_cast<integer>(123)));
	});
}

BOOST_AUTO_TEST_CASE(return_null_compile_test)
{
	std::string const source = "{ return null }";
	std::vector<value> const arguments;
	run_valid_source(source, arguments,
					 [](value const &result, p0::intermediate::unit const &program)
	{
		BOOST_CHECK(is_null(result));
	});
}

BOOST_AUTO_TEST_CASE(return_string_compile_test)
{
	std::string const source = "{ return \"hello, world!\" }";
	std::vector<value> const arguments;
	run_valid_source(source, arguments,
					 [](value const &result, p0::intermediate::unit const &program)
	{
		BOOST_REQUIRE(result.type == value_type::object);
		BOOST_REQUIRE(dynamic_cast<string const *>(result.obj));
		BOOST_CHECK(dynamic_cast<string const &>(*result.obj).content() == "hello, world!");
	});
}

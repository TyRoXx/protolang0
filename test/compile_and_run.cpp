#include "p0run/interpreter.hpp"
#include "p0compile/compiler.hpp"
#include <boost/test/unit_test.hpp>


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
			std::vector<p0::run::value> const &arguments,
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
		BOOST_CHECK(result == p0::run::value(main_function));
	});
}

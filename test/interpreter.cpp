#include "p0run/interpreter.hpp"
#include "p0i/function.hpp"
#include "p0i/emitter.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/foreach.hpp>
using namespace p0::run;
using namespace p0;


namespace
{
	template <class FunctionCreator, class ResultHandler>
	void run_single_function(
		FunctionCreator const &create,
		std::vector<value> const &arguments,
		ResultHandler const &handle_result
		)
	{
		intermediate::unit::function_vector functions;
		intermediate::emitter::instruction_vector instructions;
		intermediate::emitter emitter(instructions);
		intermediate::unit::string_vector strings;
		create(emitter, strings);
		functions.push_back(intermediate::function(instructions, arguments.size()));
		intermediate::unit program(functions, strings, 64);
		interpreter interpreter(program);
		auto const result = interpreter.call(program.functions()[0], arguments);
		handle_result(result);
	}
}

BOOST_AUTO_TEST_CASE(nothing_operation_test)
{
	for (size_t i = 0; i < 3; ++i)
	{
		run_single_function(
			[i](intermediate::emitter &emitter, intermediate::unit::string_vector &strings)
		{
			for (size_t j = 0; j < i; ++j)
			{
				emitter.nothing();
			}
		},
			std::vector<value>(),
			[](value const &result)
		{
			BOOST_CHECK(result == value());
		});
	}
}

BOOST_AUTO_TEST_CASE(set_from_constant_operation_test)
{
	static std::array<integer, 5> const test_numbers =
	{{
		0,
		1,
		-34,
		std::numeric_limits<integer>::min(),
		std::numeric_limits<integer>::max()
	}};

	BOOST_FOREACH (integer const number, test_numbers)
	{
		run_single_function(
			[number](intermediate::emitter &emitter, intermediate::unit::string_vector &)
		{
			emitter.set_from_constant(0, number);
		},
			std::vector<value>(),
			[number](value const &result)
		{
			BOOST_CHECK(result == value(number));
		});
	}
}

#include "p0run/interpreter.hpp"
#include "p0run/string.hpp"
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
		intermediate::unit program(functions, strings);
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
			[i](intermediate::emitter &emitter, intermediate::unit::string_vector &)
		{
			for (size_t j = 0; j < i; ++j)
			{
				emitter.nothing();
			}
		},
			std::vector<value>(),
			[](value const &result)
		{
			//the function pointer should still be there because we
			//didn't overwrite it
			BOOST_CHECK(result.type == value_type::function_ptr);
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

BOOST_AUTO_TEST_CASE(set_null_operation_test)
{
	run_single_function(
		[](intermediate::emitter &emitter, intermediate::unit::string_vector &)
	{
		emitter.set_from_constant(1, 123);

		//overwrite the "123" with null
		emitter.set_null(1);

		//return the null
		emitter.copy(0, 1);
	},
		std::vector<value>(),
		[](value const &result)
	{
		BOOST_CHECK(result == value());
	});
}

BOOST_AUTO_TEST_CASE(copy_operation_test)
{
	value const test_value(static_cast<integer>(6));

	run_single_function(
		[](intermediate::emitter &emitter, intermediate::unit::string_vector &)
	{
		//return the first argument
		emitter.copy(0, 1);
	},
		std::vector<value>(1, test_value),
		[test_value](value const &result)
	{
		BOOST_CHECK(result == test_value);
	});
}

namespace
{
	void integer_arithmetic_test(
		intermediate::instruction_type::Enum op,
		integer left,
		integer right,
		integer expected_result
		)
	{
		std::vector<value> arguments;
		arguments.push_back(value(left));
		arguments.push_back(value(right));

		run_single_function(
			[op](intermediate::emitter &emitter, intermediate::unit::string_vector &)
		{
			//[1] += [2]
			emitter.push_instruction(intermediate::instruction(op, 1, 2));
			//[0] = [1]
			emitter.copy(0, 1);
		},
			arguments,
			[expected_result](value const &result)
		{
			BOOST_CHECK(result == value(expected_result));
		});
	}
}

BOOST_AUTO_TEST_CASE(add_operation_test)
{
	using namespace intermediate::instruction_type;
	integer_arithmetic_test(add,  60,  70,  130);
	integer_arithmetic_test(add,   0,  70,   70);
	integer_arithmetic_test(add,  60,   0,   60);
	integer_arithmetic_test(add, -60,  70,   10);
	integer_arithmetic_test(add, -60, -70, -130);
}

BOOST_AUTO_TEST_CASE(sub_operation_test)
{
	using namespace intermediate::instruction_type;
	integer_arithmetic_test(sub,  60,  70,  -10);
	integer_arithmetic_test(sub,   0,  70,  -70);
	integer_arithmetic_test(sub,  60,   0,   60);
	integer_arithmetic_test(sub, -60,  70, -130);
	integer_arithmetic_test(sub, -60, -70,   10);
}

BOOST_AUTO_TEST_CASE(mul_operation_test)
{
	using namespace intermediate::instruction_type;
	integer_arithmetic_test(mul,  60,  70,  4200);
	integer_arithmetic_test(mul,   0,  70,     0);
	integer_arithmetic_test(mul,  60,   0,     0);
	integer_arithmetic_test(mul, -60,  70, -4200);
	integer_arithmetic_test(mul, -60, -70,  4200);
	integer_arithmetic_test(mul,   1,  70,    70);
	integer_arithmetic_test(mul,  60,   1,    60);
}

BOOST_AUTO_TEST_CASE(div_operation_test)
{
	using namespace intermediate;
	//VC++ says 'div' is ambiguous
	integer_arithmetic_test(instruction_type::div,  60,  70,   0);
	integer_arithmetic_test(instruction_type::div,   0,  70,   0);
	integer_arithmetic_test(instruction_type::div,  60,   2,  30);
	integer_arithmetic_test(instruction_type::div, -70,  60,  -1);
	integer_arithmetic_test(instruction_type::div, -70, -60,   1);
	integer_arithmetic_test(instruction_type::div,   1,  70,   0);
	integer_arithmetic_test(instruction_type::div,  60,   1,  60);

	BOOST_CHECK_EXCEPTION(integer_arithmetic_test(instruction_type::div, 1, 0, -1),
		std::runtime_error,
		[](std::runtime_error const &) { return true; });
}

BOOST_AUTO_TEST_CASE(mod_operation_test)
{
	using namespace intermediate::instruction_type;
	integer_arithmetic_test(mod,  60,  70,  60);
	integer_arithmetic_test(mod,   0,  70,   0);
	integer_arithmetic_test(mod,  60,   2,   0);
	integer_arithmetic_test(mod, -70,  60, -10);
	integer_arithmetic_test(mod,   1,  70,   1);
	integer_arithmetic_test(mod,  60,   1,   0);

	BOOST_CHECK_EXCEPTION(integer_arithmetic_test(mod, 1, 0, -1),
		std::runtime_error,
		[](std::runtime_error const &) { return true; });
}

BOOST_AUTO_TEST_CASE(and_operation_test)
{
	using namespace intermediate::instruction_type;
	integer_arithmetic_test(and_,  60,  70,  60 & 70);
	integer_arithmetic_test(and_,   0,  70,   0 & 70);
	integer_arithmetic_test(and_,  60,   2,  60 &  2);
	integer_arithmetic_test(and_, -70,  60, -70 & 60);
	integer_arithmetic_test(and_,   1,  70,   1 & 70);
	integer_arithmetic_test(and_,  60,   1,  60 &  1);
}

BOOST_AUTO_TEST_CASE(or_operation_test)
{
	using namespace intermediate::instruction_type;
	integer_arithmetic_test(or_,  60,  70,  60 | 70);
	integer_arithmetic_test(or_,   0,  70,   0 | 70);
	integer_arithmetic_test(or_,  60,   2,  60 |  2);
	integer_arithmetic_test(or_, -70,  60, -70 | 60);
	integer_arithmetic_test(or_,   1,  70,   1 | 70);
	integer_arithmetic_test(or_,  60,   1,  60 |  1);
}

BOOST_AUTO_TEST_CASE(xor_operation_test)
{
	using namespace intermediate::instruction_type;
	integer_arithmetic_test(xor_,  60,  70,  60 ^ 70);
	integer_arithmetic_test(xor_,   0,  70,   0 ^ 70);
	integer_arithmetic_test(xor_,  60,   2,  60 ^  2);
	integer_arithmetic_test(xor_, -70,  60, -70 ^ 60);
	integer_arithmetic_test(xor_,   1,  70,   1 ^ 70);
	integer_arithmetic_test(xor_,  60,   1,  60 ^  1);
}

BOOST_AUTO_TEST_CASE(shift_left_operation_test)
{
	using namespace intermediate::instruction_type;
	integer_arithmetic_test(shift_left,  60,   7,  60 << 7);
	integer_arithmetic_test(shift_left,   0,   7,   0 << 7);
	integer_arithmetic_test(shift_left,  60,   2,  60 << 2);
	integer_arithmetic_test(shift_left,  70,   6,  70 << 6);
	integer_arithmetic_test(shift_left,   1,   7,   1 << 7);
	integer_arithmetic_test(shift_left,  60,   1,  60 << 1);
}

BOOST_AUTO_TEST_CASE(shift_right_operation_test)
{
	using namespace intermediate::instruction_type;
	integer_arithmetic_test(shift_right,  60,   7,  60 >> 7);
	integer_arithmetic_test(shift_right,   0,   7,   0 >> 7);
	integer_arithmetic_test(shift_right,  60,   2,  60 >> 2);
	integer_arithmetic_test(shift_right,  70,   6,  70 >> 6);
	integer_arithmetic_test(shift_right,   1,   7,   1 >> 7);
	integer_arithmetic_test(shift_right,  60,   1,  60 >> 1);
}

BOOST_AUTO_TEST_CASE(recursion_test)
{
	std::vector<value> arguments;
	arguments.push_back(value(static_cast<integer>(6)));
	arguments.push_back(value(static_cast<integer>(0)));

	run_single_function(
		[](intermediate::emitter &emitter, intermediate::unit::string_vector &)
	{
		//0
		emitter.set_from_constant(3, 0);
		emitter.less(3, 1);
		emitter.jump_if_not(12, 3);
		emitter.copy(3, 0);
		emitter.copy(4, 1);
		//5
		emitter.set_from_constant(6, 1);
		emitter.sub(4, 6);
		emitter.copy(5, 2);
		emitter.set_from_constant(6, 3);
		emitter.add(5, 6);
		//10
		emitter.call(3, 2);
		emitter.copy(2, 3);
		emitter.copy(0, 2);
	},
		arguments,
		[](value const &result)
	{
		BOOST_CHECK(result == value(static_cast<integer>(6 * 3)));
	});
}

namespace
{
	template <class NonFunctionEmitter>
	void test_call_non_function(NonFunctionEmitter const &nonFunctionEmitter)
	{
		BOOST_CHECK_EXCEPTION((
			run_single_function(
			[nonFunctionEmitter](intermediate::emitter &emitter, intermediate::unit::string_vector &)
		{
			nonFunctionEmitter(emitter);
			emitter.call(1, 0);
		},
			std::vector<value>(),
			[](value const & /*result*/)
		{
			throw std::runtime_error("The function is not expected to return anything");
		})),
			std::runtime_error,
			[](std::runtime_error const &) { return true; });
	}
}

BOOST_AUTO_TEST_CASE(call_non_function_test)
{
	test_call_non_function([](intermediate::emitter &emitter)
	{
		emitter.set_from_constant(1, 123);
	});
	test_call_non_function([](intermediate::emitter &emitter)
	{
		emitter.set_null(1);
	});
	test_call_non_function([](intermediate::emitter &emitter)
	{
		emitter.new_table(1);
	});
}

BOOST_AUTO_TEST_CASE(string_literal_test)
{
	std::string const test_string = "hello, world!";

	run_single_function(
		[&test_string](
		intermediate::emitter &emitter,
		intermediate::unit::string_vector &strings)
	{
		strings.push_back(test_string);
		emitter.set_string(0, 0);
	},
		std::vector<value>(),
		[&test_string](value const &result)
	{
		BOOST_REQUIRE(result.type == value_type::object);
		BOOST_REQUIRE(dynamic_cast<string *>(result.obj));
		BOOST_CHECK(dynamic_cast<string &>(*result.obj).content() == test_string);
	});
}

BOOST_AUTO_TEST_CASE(string_get_element_test)
{
	std::string const test_string = "hello, world!";

	run_single_function(
		[&test_string](
		intermediate::emitter &emitter,
		intermediate::unit::string_vector &strings)
	{
		strings.push_back(test_string);
		emitter.set_string(1, 0);
		emitter.set_from_constant(2, 12);
		emitter.get_element(1, 2, 0);
	},
		std::vector<value>(),
		[&test_string](value const &result)
	{
		BOOST_CHECK(result == value(static_cast<integer>('!')));
	});
}

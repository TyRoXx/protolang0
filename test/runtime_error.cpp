#include "p0run/interpreter.hpp"
#include "p0run/default_garbage_collector.hpp"
#include "p0run/runtime_error.hpp"
#include "p0run/runtime_error_exception.hpp"
#include "p0i/emitter.hpp"
#include <boost/test/unit_test.hpp>
using namespace p0::run;
using namespace p0;


namespace
{
	template <class FunctionCreator, class ResultHandler, class LoadModule>
	void run_single_function(
		FunctionCreator const &create,
		std::vector<value> const &arguments,
		ResultHandler const &handle_result,
		LoadModule const &load_module
		)
	{
		intermediate::unit::function_vector functions;
		intermediate::emitter::instruction_vector instructions;
		intermediate::emitter emitter(instructions);
		intermediate::unit::string_vector strings;
		create(emitter, strings);
		functions.push_back(intermediate::function(instructions, arguments.size(), 0));
		intermediate::unit program(functions, strings);
		run::default_garbage_collector gc;
		run::interpreter interpreter(gc, load_module);
		auto const result = interpreter.call(
				intermediate::function_ref(program, program.functions()[0]),
				arguments);
		handle_result(result);
	}

	template <class FunctionCreator, class ResultHandler>
	void run_single_function(
		FunctionCreator const &create,
		std::vector<value> const &arguments,
		ResultHandler const &handle_result
		)
	{
		run_single_function(create, arguments, handle_result, nullptr);
	}
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
			p0::run::runtime_error_exception,
			[](p0::run::runtime_error_exception const &) { return true; });
	}
}

BOOST_AUTO_TEST_CASE(call_non_function_test)
{
	test_call_non_function([](intermediate::emitter &emitter)
	{
		emitter.set_constant(1, 123);
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

namespace
{
	template <class Emit>
	void expect_runtime_error(Emit const &emit)
	{
		bool found_error = false;
		BOOST_CHECK_EXCEPTION((
		run_single_function(
			emit,
			std::vector<value>(),
			[](value const &) -> void
		{
			BOOST_CHECK(nullptr == "No result expected");
		})),
			p0::run::runtime_error_exception,
			([&found_error](p0::run::runtime_error_exception const &) -> bool
		{
			BOOST_REQUIRE(!found_error);
			found_error = true;
			return true;
		}));
		BOOST_CHECK(found_error);
	}

	template <class Emit>
	void expect_arithmetic_error(Emit const &emit)
	{
		expect_runtime_error(emit);
	}

	template <class Emit>
	void expect_runtime_error(p0::run::runtime_error_code::type expected_error,
							  Emit const &emit)
	{
		BOOST_CHECK_EXCEPTION((
		run_single_function(
			emit,
			std::vector<value>(),
			[](value const &) -> void
		{
			BOOST_CHECK(nullptr == "No result expected");
		})),
			p0::run::runtime_error_exception,
			([expected_error](p0::run::runtime_error_exception const &ex) -> bool
		{
			BOOST_REQUIRE(ex.error().type() == expected_error);
			return true;
		}));
	}
}

BOOST_AUTO_TEST_CASE(divide_by_zero_test)
{
	expect_arithmetic_error([](
		intermediate::emitter &emitter,
		intermediate::unit::string_vector &)
	{
		emitter.set_constant(1, 42);
		emitter.set_constant(2, 0);
		emitter.div(1, 2);
	});

	//only difference: mod instead of div
	expect_arithmetic_error([](
		intermediate::emitter &emitter,
		intermediate::unit::string_vector &)
	{
		emitter.set_constant(1, 42);
		emitter.set_constant(2, 0);
		emitter.mod(1, 2);
	});
}

BOOST_AUTO_TEST_CASE(division_overflow_test)
{
	expect_arithmetic_error([](
		intermediate::emitter &emitter,
		intermediate::unit::string_vector &)
	{
		emitter.set_constant(1, std::numeric_limits<run::integer>::min());
		emitter.set_constant(2, -1);
		emitter.div(1, 2);
	});

	//only difference: mod instead of div
	expect_arithmetic_error([](
		intermediate::emitter &emitter,
		intermediate::unit::string_vector &)
	{
		emitter.set_constant(1, std::numeric_limits<run::integer>::min());
		emitter.set_constant(2, -1);
		emitter.mod(1, 2);
	});
}

namespace
{
	void test_invalid_shift_amount(run::integer amount,
								   intermediate::instruction_type::Enum shift)
	{
		expect_arithmetic_error([amount, shift](
			intermediate::emitter &emitter,
			intermediate::unit::string_vector &)
		{
			emitter.set_constant(1, 123);
			emitter.set_constant(2, amount);
			emitter.push_instruction(intermediate::instruction(shift, 1, 2));
		});
	}

	void test_negative_invalid_shift(intermediate::instruction_type::Enum operation)
	{
		test_invalid_shift_amount(-1, operation);
		test_invalid_shift_amount(-2, operation);
		test_invalid_shift_amount(-64, operation);
		test_invalid_shift_amount(std::numeric_limits<run::integer>::min(), operation);
	}
}

BOOST_AUTO_TEST_CASE(negative_shift_amount_test)
{
	using namespace intermediate::instruction_type;

	test_negative_invalid_shift(shift_left);
	test_negative_invalid_shift(shift_right);
	test_negative_invalid_shift(shift_signed);
}

namespace
{
	void test_big_shift(intermediate::instruction_type::Enum operation)
	{
		run::integer const max_shift = sizeof(run::integer) * CHAR_BIT - 1;
		test_invalid_shift_amount(max_shift + 1, operation);
		test_invalid_shift_amount(max_shift * 2, operation);
		test_invalid_shift_amount(max_shift + 64, operation);
		test_invalid_shift_amount(std::numeric_limits<run::integer>::max(), operation);
	}
}

BOOST_AUTO_TEST_CASE(big_shift_amount_test)
{
	using namespace intermediate::instruction_type;

	test_big_shift(shift_left);
	test_big_shift(shift_right);
	test_big_shift(shift_signed);
}

namespace
{
	void test_add_overflow(run::integer left, run::integer right,
						   intermediate::instruction_type::Enum add)
	{
		expect_arithmetic_error([left, right, add](
			intermediate::emitter &emitter,
			intermediate::unit::string_vector &)
		{
			emitter.set_constant(1, left);
			emitter.set_constant(2, right);
			emitter.push_instruction(intermediate::instruction(add, 1, 2));
		});
	}
}

BOOST_AUTO_TEST_CASE(add_overflow_test)
{
	using intermediate::instruction_type::add;

	auto const max_int = std::numeric_limits<run::integer>::max();

	test_add_overflow(max_int, 1, add);
	test_add_overflow(max_int - 66, 1 + 66, add);
	test_add_overflow(max_int / 2 + 1, max_int / 2 + 1, add);
}

BOOST_AUTO_TEST_CASE(sub_overflow_test)
{
	using intermediate::instruction_type::sub;

	auto const min_int = std::numeric_limits<run::integer>::min();

	test_add_overflow(min_int, 1, sub);
	test_add_overflow(min_int + 66, 1 + 66, sub);
	test_add_overflow(min_int / 2, - (min_int / 2) + 1, sub);
}

BOOST_AUTO_TEST_CASE(negation_overflow_test)
{
	expect_arithmetic_error([](
		intermediate::emitter &emitter,
		intermediate::unit::string_vector &)
	{
		emitter.set_constant(1, std::numeric_limits<run::integer>::min());
		emitter.negate(1);
	});
}

BOOST_AUTO_TEST_CASE(load_module_invalid_argument_test)
{
	expect_runtime_error([](intermediate::emitter &emitter,
							intermediate::unit::string_vector &)
	{
		emitter.set_null(0);
		emitter.load_module(0);
	});

	expect_runtime_error([](intermediate::emitter &emitter,
							intermediate::unit::string_vector &)
	{
		emitter.set_constant(0, 123);
		emitter.load_module(0);
	});

	expect_runtime_error([](intermediate::emitter &emitter,
							intermediate::unit::string_vector &)
	{
		emitter.new_table(0);
		emitter.load_module(0);
	});
}

BOOST_AUTO_TEST_CASE(call_method_misuse_test)
{
	expect_runtime_error([](intermediate::emitter &emitter,
							intermediate::unit::string_vector &)
	{
		emitter.set_null(3); //name
		emitter.set_null(1); //instance
		emitter.call_method(1, 0, 3);
	});

	expect_runtime_error([](intermediate::emitter &emitter,
							intermediate::unit::string_vector &strings)
	{
		strings.push_back("not a class instance");
		emitter.set_null(3); //name
		emitter.set_string(1, 0); //instance
		emitter.call_method(1, 0, 3);
	});

	expect_runtime_error([](intermediate::emitter &emitter,
							intermediate::unit::string_vector &)
	{
		emitter.set_null(3); //name
		emitter.set_constant(1, 123); //instance
		emitter.call_method(1, 0, 3);
	});
}

BOOST_AUTO_TEST_CASE(jump_misuse_test)
{
	expect_runtime_error(
				p0::run::runtime_error_code::jump_out_of_range,
				[](intermediate::emitter &emitter,
				   intermediate::unit::string_vector &)
	{
		emitter.jump(9001);
	});

	//we except an error even if the branch is not taken
	expect_runtime_error(
				p0::run::runtime_error_code::jump_out_of_range,
				[](intermediate::emitter &emitter,
				   intermediate::unit::string_vector &)
	{
		emitter.set_constant(0, 0);
		emitter.jump_if(9001, 0);
	});

	expect_runtime_error(
				p0::run::runtime_error_code::jump_out_of_range,
				[](intermediate::emitter &emitter,
				   intermediate::unit::string_vector &)
	{
		emitter.set_constant(0, 1);
		emitter.jump_if_not(9001, 0);
	});
}

#include "p0run/interpreter.hpp"
#include "p0run/string.hpp"
#include "p0run/table.hpp"
#include "p0i/function.hpp"
#include "p0i/emitter.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/foreach.hpp>
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
		interpreter interpreter(program, load_module);
		auto const result = interpreter.call(program.functions()[0], arguments);
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
		BOOST_CHECK(is_null(result));
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

BOOST_AUTO_TEST_CASE(string_get_element_operation_test)
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

BOOST_AUTO_TEST_CASE(new_table_operation_test)
{
	run_single_function(
		[](
		intermediate::emitter &emitter,
		intermediate::unit::string_vector &)
	{
		emitter.new_table(0);
	},
		std::vector<value>(),
		[](value const &result)
	{
		BOOST_REQUIRE(result.type == value_type::object);
		BOOST_REQUIRE(dynamic_cast<table *>(result.obj));
	});
}

BOOST_AUTO_TEST_CASE(table_get_element_operation_test)
{
	run_single_function(
		[](
		intermediate::emitter &emitter,
		intermediate::unit::string_vector &)
	{
		emitter.new_table(1);
		emitter.set_from_constant(2, 123);
		emitter.get_element(1, 2, 0);
	},
		std::vector<value>(),
		[](value const &result)
	{
		BOOST_CHECK(is_null(result));
	});
}

BOOST_AUTO_TEST_CASE(table_set_element_operation_test)
{
	run_single_function(
		[](
		intermediate::emitter &emitter,
		intermediate::unit::string_vector &)
	{
		emitter.new_table(1);

		emitter.set_from_constant(2, 123);
		emitter.set_from_constant(3, 456);
		emitter.set_element(1, 2, 3);

		emitter.set_from_constant(2, 124);
		emitter.set_from_constant(3, -7);
		emitter.set_element(1, 2, 3);

		emitter.set_from_constant(2, 123);
		emitter.get_element(1, 2, 0);
	},
		std::vector<value>(),
		[](value const &result)
	{
		BOOST_CHECK(result == value(static_cast<integer>(456)));
	});
}

BOOST_AUTO_TEST_CASE(missing_argument_test)
{
	run_single_function(
		[](
		intermediate::emitter &emitter,
		intermediate::unit::string_vector &)
	{
		emitter.set_from_constant(2, 44);
		emitter.equal(1, 2);
		emitter.jump_if_not(6, 1);
		emitter.copy(3, 0);
		emitter.copy(4, 2);
		//5
		emitter.call(3, 0);
		emitter.set_null(0);
	},
		std::vector<value>(1, value(static_cast<integer>(44))),
		[](value const &result)
	{
		BOOST_CHECK(is_null(result));
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
			std::runtime_error,
			([&found_error](std::runtime_error const &) -> bool
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
}

BOOST_AUTO_TEST_CASE(divide_by_zero_test)
{
	expect_arithmetic_error([](
		intermediate::emitter &emitter,
		intermediate::unit::string_vector &)
	{
		emitter.set_from_constant(1, 42);
		emitter.set_from_constant(2, 0);
		emitter.div(1, 2);
	});

	//only difference: mod instead of div
	expect_arithmetic_error([](
		intermediate::emitter &emitter,
		intermediate::unit::string_vector &)
	{
		emitter.set_from_constant(1, 42);
		emitter.set_from_constant(2, 0);
		emitter.mod(1, 2);
	});
}

BOOST_AUTO_TEST_CASE(division_overflow_test)
{
	expect_arithmetic_error([](
		intermediate::emitter &emitter,
		intermediate::unit::string_vector &)
	{
		emitter.set_from_constant(1, std::numeric_limits<integer>::min());
		emitter.set_from_constant(2, -1);
		emitter.div(1, 2);
	});

	//only difference: mod instead of div
	expect_arithmetic_error([](
		intermediate::emitter &emitter,
		intermediate::unit::string_vector &)
	{
		emitter.set_from_constant(1, std::numeric_limits<integer>::min());
		emitter.set_from_constant(2, -1);
		emitter.mod(1, 2);
	});
}

namespace
{
	void test_invalid_shift_amount(integer amount,
								   intermediate::instruction_type::Enum shift)
	{
		expect_arithmetic_error([amount, shift](
			intermediate::emitter &emitter,
			intermediate::unit::string_vector &)
		{
			emitter.set_from_constant(1, 123);
			emitter.set_from_constant(2, amount);
			emitter.push_instruction(intermediate::instruction(shift, 1, 2));
		});
	}

	void test_negative_invalid_shift(intermediate::instruction_type::Enum operation)
	{
		test_invalid_shift_amount(-1, operation);
		test_invalid_shift_amount(-2, operation);
		test_invalid_shift_amount(-64, operation);
		test_invalid_shift_amount(std::numeric_limits<integer>::min(), operation);
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
		integer const max_shift = sizeof(integer) * CHAR_BIT - 1;
		test_invalid_shift_amount(max_shift + 1, operation);
		test_invalid_shift_amount(max_shift * 2, operation);
		test_invalid_shift_amount(max_shift + 64, operation);
		test_invalid_shift_amount(std::numeric_limits<integer>::max(), operation);
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
	void test_add_overflow(integer left,
						   integer right,
						   intermediate::instruction_type::Enum add)
	{
		expect_arithmetic_error([left, right, add](
			intermediate::emitter &emitter,
			intermediate::unit::string_vector &)
		{
			emitter.set_from_constant(1, left);
			emitter.set_from_constant(2, right);
			emitter.push_instruction(intermediate::instruction(add, 1, 2));
		});
	}
}

BOOST_AUTO_TEST_CASE(add_overflow_test)
{
	using intermediate::instruction_type::add;

	auto const max_int = std::numeric_limits<integer>::max();

	test_add_overflow(max_int, 1, add);
	test_add_overflow(max_int - 66, 1 + 66, add);
	test_add_overflow(max_int / 2 + 1, max_int / 2 + 1, add);
}

BOOST_AUTO_TEST_CASE(sub_overflow_test)
{
	using intermediate::instruction_type::sub;

	auto const min_int = std::numeric_limits<integer>::min();

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
		emitter.set_from_constant(1, std::numeric_limits<integer>::min());
		emitter.negate(1);
	});
}

BOOST_AUTO_TEST_CASE(modules_disabled_test)
{
	//modules are disabled by default, so we expect load_module to return null
	run_single_function(
		[](
		intermediate::emitter &emitter,
		intermediate::unit::string_vector &strings)
	{
		strings.push_back("module_name");
		emitter.set_string(0, 0);
		emitter.load_module(0);
	},
		std::vector<value>(),
		[](value const &result)
	{
		BOOST_CHECK(result == value());
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
		emitter.set_from_constant(0, 123);
		emitter.load_module(0);
	});

	expect_runtime_error([](intermediate::emitter &emitter,
							intermediate::unit::string_vector &)
	{
		emitter.new_table(0);
		emitter.load_module(0);
	});
}

namespace
{
	struct trivial_module : p0::run::object
	{
		virtual boost::optional<value> get_element(value const & /*key*/) const override
		{
			return value(static_cast<integer>(456));
		}

		virtual bool set_element(value const & /*key*/, value const & /*value*/) override
		{
			BOOST_REQUIRE(nullptr == "This module is read-only");
			return false;
		}

	private:

		virtual void mark_recursively() override
		{
		}
	};
}

BOOST_AUTO_TEST_CASE(load_trivial_module_test)
{
	std::string const module_name = "trivial";
	bool module_loaded = false;
	p0::run::object *module_ptr = nullptr;

	run_single_function(
		[&module_name](
		intermediate::emitter &emitter,
		intermediate::unit::string_vector &strings)
	{
		strings.push_back(module_name);
		emitter.set_string(0, 0);
		emitter.load_module(0);
	},
		std::vector<value>(),
		[&module_ptr](value const &result)
	{
		BOOST_REQUIRE(result.type == value_type::object);
		BOOST_REQUIRE(result.obj == module_ptr);
		BOOST_CHECK(result.obj->get_element(value()) == value(static_cast<integer>(456)));
	},
		[&module_name, &module_loaded, &module_ptr](interpreter &, std::string const &name)
			-> std::unique_ptr<p0::run::object>
	{
		BOOST_REQUIRE(!module_loaded);
		BOOST_CHECK(name == module_name);
		std::unique_ptr<p0::run::object> module(new trivial_module);
		module_ptr = module.get();
		module_loaded = true;
		return module;
	});

	BOOST_CHECK(module_loaded);
}

#include "p0optimize/remove_no_ops.hpp"
#include "p0i/emitter.hpp"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(remove_no_ops_keeps_return_value)
{
	p0::intermediate::function::instruction_vector original_code;
	{
		p0::intermediate::emitter emitter(original_code);
		emitter.set_constant(0, 2);
		emitter.set_constant(0, 3);
	}
	p0::intermediate::function::instruction_vector const cleaned_code = p0::remove_no_ops(original_code);
	p0::intermediate::function::instruction_vector expected_code;
	{
		p0::intermediate::emitter emitter(expected_code);
		//the value at 0 (the call's result) should not go away
		emitter.set_constant(0, 3);
	}
	BOOST_CHECK(expected_code == cleaned_code);
}

BOOST_AUTO_TEST_CASE(remove_no_ops_set_from_constant)
{
	p0::intermediate::function::instruction_vector original_code;
	{
		p0::intermediate::emitter emitter(original_code);
		emitter.set_constant(1, 2);
		emitter.set_constant(2, 3);
	}
	p0::intermediate::function::instruction_vector const cleaned_code = p0::remove_no_ops(original_code);
	p0::intermediate::function::instruction_vector expected_code;
	BOOST_CHECK(expected_code == cleaned_code);
}

BOOST_AUTO_TEST_CASE(remove_no_ops_nothing)
{
	p0::intermediate::function::instruction_vector original_code;
	{
		p0::intermediate::emitter emitter(original_code);
		emitter.nothing();
	}
	p0::intermediate::function::instruction_vector const cleaned_code = p0::remove_no_ops(original_code);
	p0::intermediate::function::instruction_vector expected_code;
	BOOST_CHECK(expected_code == cleaned_code);
}

BOOST_AUTO_TEST_CASE(remove_no_ops_jump_1)
{
	p0::intermediate::function::instruction_vector original_code;
	{
		p0::intermediate::emitter emitter(original_code);
		emitter.jump(1);
	}
	p0::intermediate::function::instruction_vector const cleaned_code = p0::remove_no_ops(original_code);
	p0::intermediate::function::instruction_vector expected_code;
	BOOST_CHECK(expected_code == cleaned_code);
}

BOOST_AUTO_TEST_CASE(remove_no_ops_jump_2)
{
	p0::intermediate::function::instruction_vector original_code;
	{
		p0::intermediate::emitter emitter(original_code);
		emitter.set_constant(0, 2);
		emitter.jump(2);
	}
	p0::intermediate::function::instruction_vector const cleaned_code = p0::remove_no_ops(original_code);
	p0::intermediate::function::instruction_vector expected_code;
	{
		p0::intermediate::emitter emitter(expected_code);
		emitter.set_constant(0, 2);
	}
	BOOST_CHECK(expected_code == cleaned_code);
}

BOOST_AUTO_TEST_CASE(remove_no_ops_empty)
{
	p0::intermediate::function::instruction_vector original_code;
	p0::intermediate::function::instruction_vector const cleaned_code = p0::remove_no_ops(original_code);
	BOOST_CHECK(original_code == cleaned_code);
}

BOOST_AUTO_TEST_CASE(remove_no_ops_add)
{
	p0::intermediate::function::instruction_vector original_code;
	{
		p0::intermediate::emitter emitter(original_code);
		emitter.set_constant(0, 2);
		emitter.set_constant(1, 3);
		emitter.add(0, 1);
		emitter.set_constant(0, 4);
	}
	p0::intermediate::function::instruction_vector const cleaned_code = p0::remove_no_ops(original_code);
	p0::intermediate::function::instruction_vector expected_code;
	{
		p0::intermediate::emitter emitter(expected_code);
		emitter.set_constant(0, 2);
		emitter.set_constant(1, 3);
		emitter.add(0, 1);
		emitter.set_constant(0, 4);
	}
	BOOST_CHECK(expected_code == cleaned_code);
}

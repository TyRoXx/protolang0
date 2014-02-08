#include "p0optimize/simplify_instructions.hpp"
#include "p0i/emitter.hpp"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(simplify_instructions_jump_if_positive)
{
	p0::intermediate::function::instruction_vector original_code;
	{
		p0::intermediate::emitter emitter(original_code);
		emitter.set_constant(1, 1);
		emitter.jump_if(2, 1);
	}
	p0::intermediate::function::instruction_vector const simplified_code = p0::simplify_instructions(original_code);
	p0::intermediate::function::instruction_vector expected_code;
	{
		p0::intermediate::emitter emitter(expected_code);
		emitter.set_constant(1, 1);
		emitter.jump(2);
	}
	BOOST_CHECK(expected_code == simplified_code);
}

BOOST_AUTO_TEST_CASE(simplify_instructions_jump_if_negative)
{
	p0::intermediate::function::instruction_vector original_code;
	{
		p0::intermediate::emitter emitter(original_code);
		emitter.set_constant(1, 0);
		emitter.jump_if(2, 1);
	}
	p0::intermediate::function::instruction_vector const simplified_code = p0::simplify_instructions(original_code);
	p0::intermediate::function::instruction_vector expected_code;
	{
		p0::intermediate::emitter emitter(expected_code);
		emitter.set_constant(1, 1);
		emitter.nothing();
	}
	BOOST_CHECK(expected_code == simplified_code);
}

BOOST_AUTO_TEST_CASE(simplify_instructions_jump_if_not_negative)
{
	p0::intermediate::function::instruction_vector original_code;
	{
		p0::intermediate::emitter emitter(original_code);
		emitter.set_constant(1, 1);
		emitter.jump_if_not(2, 1);
	}
	p0::intermediate::function::instruction_vector const simplified_code = p0::simplify_instructions(original_code);
	p0::intermediate::function::instruction_vector expected_code;
	{
		p0::intermediate::emitter emitter(expected_code);
		emitter.set_constant(1, 1);
		emitter.nothing();
	}
	BOOST_CHECK(expected_code == simplified_code);
}

BOOST_AUTO_TEST_CASE(simplify_instructions_jump_if_not_positive)
{
	p0::intermediate::function::instruction_vector original_code;
	{
		p0::intermediate::emitter emitter(original_code);
		emitter.set_constant(1, 0);
		emitter.jump_if_not(2, 1);
	}
	p0::intermediate::function::instruction_vector const simplified_code = p0::simplify_instructions(original_code);
	p0::intermediate::function::instruction_vector expected_code;
	{
		p0::intermediate::emitter emitter(expected_code);
		emitter.set_constant(1, 1);
		emitter.jump(2);
	}
	BOOST_CHECK(expected_code == simplified_code);
}

BOOST_AUTO_TEST_CASE(simplify_instructions_copy)
{
	p0::intermediate::function::instruction_vector original_code;
	{
		p0::intermediate::emitter emitter(original_code);
		emitter.set_constant(1, 2);
		emitter.copy(0, 1);
	}
	p0::intermediate::function::instruction_vector const simplified_code = p0::simplify_instructions(original_code);
	p0::intermediate::function::instruction_vector expected_code;
	{
		p0::intermediate::emitter emitter(expected_code);
		emitter.set_constant(1, 2);
		emitter.set_constant(0, 2);
	}
	BOOST_CHECK(expected_code == simplified_code);
}

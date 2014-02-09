#include "p0optimize/remove_dead_code.hpp"
#include "p0i/emitter.hpp"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(remove_dead_code_empty)
{
	p0::intermediate::function::instruction_vector original_code;
	p0::intermediate::function::instruction_vector const living_code = p0::remove_dead_code(original_code);
	p0::intermediate::function::instruction_vector expected_code;
	BOOST_CHECK(expected_code == living_code);
}

BOOST_AUTO_TEST_CASE(remove_dead_code_jump)
{
	p0::intermediate::function::instruction_vector original_code;
	{
		p0::intermediate::emitter emitter(original_code);
		emitter.set_constant(0, 2);
		emitter.jump(3); //return
		emitter.set_constant(0, 3);
	}
	p0::intermediate::function original_function(original_code, 0, 0);
	p0::intermediate::function::instruction_vector const living_code = p0::remove_dead_code(original_function.body());
	p0::intermediate::function::instruction_vector expected_code;
	{
		p0::intermediate::emitter emitter(expected_code);
		emitter.set_constant(0, 2);
		emitter.jump(2);
		//the redundant jump stays here because the dead code pass only removes instructions that are not executed
	}
	BOOST_CHECK(expected_code == living_code);
}

BOOST_AUTO_TEST_CASE(remove_dead_code_jump_if)
{
	p0::intermediate::function::instruction_vector original_code;
	{
		p0::intermediate::emitter emitter(original_code);
		emitter.set_constant(0, 2);
		emitter.jump_if(3, 1);
		emitter.set_constant(0, 3); //nothing is dead here
	}
	p0::intermediate::function original_function(original_code, 0, 0);
	p0::intermediate::function::instruction_vector const living_code = p0::remove_dead_code(original_function.body());
	BOOST_CHECK(original_code == living_code);
}

BOOST_AUTO_TEST_CASE(remove_dead_code_jump_if_not)
{
	p0::intermediate::function::instruction_vector original_code;
	{
		p0::intermediate::emitter emitter(original_code);
		emitter.set_constant(0, 2);
		emitter.jump_if_not(3, 1);
		emitter.set_constant(0, 3); //nothing is dead here
	}
	p0::intermediate::function original_function(original_code, 0, 0);
	p0::intermediate::function::instruction_vector const living_code = p0::remove_dead_code(original_function.body());
	BOOST_CHECK(original_code == living_code);
}

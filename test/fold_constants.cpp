#include "p0optimize/fold_constants.hpp"
#include "p0optimize/remove_no_ops.hpp"
#include "p0i/emitter.hpp"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(fold_constants_add)
{
	p0::intermediate::function::instruction_vector original_code;
	{
		p0::intermediate::emitter emitter(original_code);
		emitter.set_constant(0, 2);
		emitter.set_constant(1, 3);
		emitter.add(0, 1);
	}
	p0::intermediate::function::instruction_vector const folded_code = p0::fold_constants(original_code);
	p0::intermediate::function::instruction_vector const cleaned_code = p0::remove_no_ops(folded_code);
	p0::intermediate::function::instruction_vector expected_code;
	{
		p0::intermediate::emitter emitter(expected_code);
		emitter.set_constant(0, 5);
	}
	BOOST_CHECK(expected_code == cleaned_code);
}

BOOST_AUTO_TEST_CASE(fold_constants_tree)
{
	p0::intermediate::function::instruction_vector original_code;
	{
		p0::intermediate::emitter emitter(original_code);
		//((2 + 3) + 1) + 1
		emitter.set_constant(0, 2);
		emitter.set_constant(1, 3);
		emitter.add(0, 1);
		emitter.set_constant(1, 1);
		emitter.add(0, 1);
		emitter.set_constant(1, 1);
		emitter.add(0, 1);
	}
	p0::intermediate::function::instruction_vector const folded_code = p0::fold_constants(original_code);
	p0::intermediate::function::instruction_vector const cleaned_code = p0::remove_no_ops(folded_code);
	p0::intermediate::function::instruction_vector expected_code;
	{
		p0::intermediate::emitter emitter(expected_code);
		emitter.set_constant(0, 7);
	}
	BOOST_CHECK(expected_code == cleaned_code);
}

BOOST_AUTO_TEST_CASE(fold_constants_do_not_follow_jump)
{
	p0::intermediate::function::instruction_vector original_code;
	{
		p0::intermediate::emitter emitter(original_code);
		/*0*/ emitter.set_constant(0, 2);
		/*1*/ emitter.set_constant(1, 3); //obsolete
		/*2*/ emitter.jump(6);
		/*3*/ emitter.set_constant(1, 4); //overwrites the write by 1
		/*5*/ emitter.add(0, 1);
		/*4*/ emitter.jump(7);
		/*6*/ emitter.jump(3);
		/*7 return*/
	}
	p0::intermediate::function::instruction_vector const folded_code = p0::fold_constants(original_code);

	//We expect fold_constants not to follow jump paths. That is the job of earlier passes.
	BOOST_CHECK(original_code == folded_code);
}

BOOST_AUTO_TEST_CASE(fold_constants_empty)
{
	p0::intermediate::function::instruction_vector original_code;
	p0::intermediate::function::instruction_vector const optimized = p0::fold_constants(original_code);
	p0::intermediate::function::instruction_vector expected_code;
	BOOST_CHECK(expected_code == optimized);
}

BOOST_AUTO_TEST_CASE(fold_constants_jump_if_positive)
{
	p0::intermediate::function::instruction_vector original_code;
	{
		p0::intermediate::emitter emitter(original_code);
		emitter.set_constant(1, 1);
		emitter.jump_if(2, 1);
	}
	p0::intermediate::function::instruction_vector const simplified_code = p0::fold_constants(original_code);
	p0::intermediate::function::instruction_vector expected_code;
	{
		p0::intermediate::emitter emitter(expected_code);
		emitter.set_constant(1, 1);
		emitter.jump(2);
	}
	BOOST_CHECK(expected_code == simplified_code);
}

BOOST_AUTO_TEST_CASE(fold_constants_jump_if_negative)
{
	p0::intermediate::function::instruction_vector original_code;
	{
		p0::intermediate::emitter emitter(original_code);
		emitter.set_constant(1, 0);
		emitter.jump_if(2, 1);
	}
	p0::intermediate::function::instruction_vector const simplified_code = p0::fold_constants(original_code);
	p0::intermediate::function::instruction_vector expected_code;
	{
		p0::intermediate::emitter emitter(expected_code);
		emitter.set_constant(1, 0);
		emitter.nothing();
	}
	BOOST_CHECK(expected_code == simplified_code);
}

BOOST_AUTO_TEST_CASE(fold_constants_jump_if_not_negative)
{
	p0::intermediate::function::instruction_vector original_code;
	{
		p0::intermediate::emitter emitter(original_code);
		emitter.set_constant(1, 1);
		emitter.jump_if_not(2, 1);
	}
	p0::intermediate::function::instruction_vector const simplified_code = p0::fold_constants(original_code);
	p0::intermediate::function::instruction_vector expected_code;
	{
		p0::intermediate::emitter emitter(expected_code);
		emitter.set_constant(1, 1);
		emitter.nothing();
	}
	BOOST_CHECK(expected_code == simplified_code);
}

BOOST_AUTO_TEST_CASE(fold_constants_jump_if_not_positive)
{
	p0::intermediate::function::instruction_vector original_code;
	{
		p0::intermediate::emitter emitter(original_code);
		emitter.set_constant(1, 0);
		emitter.jump_if_not(2, 1);
	}
	p0::intermediate::function::instruction_vector const simplified_code = p0::fold_constants(original_code);
	p0::intermediate::function::instruction_vector expected_code;
	{
		p0::intermediate::emitter emitter(expected_code);
		emitter.set_constant(1, 0);
		emitter.jump(2);
	}
	BOOST_CHECK(expected_code == simplified_code);
}

BOOST_AUTO_TEST_CASE(fold_constants_copy)
{
	p0::intermediate::function::instruction_vector original_code;
	{
		p0::intermediate::emitter emitter(original_code);
		emitter.set_constant(1, 2);
		emitter.copy(0, 1);
	}
	p0::intermediate::function::instruction_vector const simplified_code = p0::fold_constants(original_code);
	p0::intermediate::function::instruction_vector expected_code;
	{
		p0::intermediate::emitter emitter(expected_code);
		emitter.set_constant(1, 2);
		emitter.set_constant(0, 2);
	}
	BOOST_CHECK(expected_code == simplified_code);
}

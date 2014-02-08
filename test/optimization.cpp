#include "p0optimize/constant_folding.hpp"
#include "p0optimize/remove_no_ops.hpp"
#include "p0optimize/remove_dead_code.hpp"
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

BOOST_AUTO_TEST_CASE(fold_constants_add)
{
	p0::intermediate::function::instruction_vector original_code;
	{
		p0::intermediate::emitter emitter(original_code);
		emitter.set_constant(0, 2);
		emitter.set_constant(1, 3);
		emitter.add(0, 1);
	}
	p0::intermediate::function original_function(original_code, 0, 0);
	p0::intermediate::function::instruction_vector const folded_code = p0::fold_constants(original_function);
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
	p0::intermediate::function original_function(original_code, 0, 0);
	p0::intermediate::function::instruction_vector const folded_code = p0::fold_constants(original_function);
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
	p0::intermediate::function original_function(original_code, 0, 0);
	p0::intermediate::function::instruction_vector const folded_code = p0::fold_constants(original_function);

	//We expect fold_constants not to follow jump paths. That is the job of earlier passes.
	BOOST_CHECK(original_code == folded_code);
}

BOOST_AUTO_TEST_CASE(remove_dead_code_empty)
{
	p0::intermediate::function::instruction_vector original_code;
	p0::intermediate::function original_function(original_code, 0, 0);
	p0::intermediate::function::instruction_vector const living_code = p0::remove_dead_code(original_function.body());
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
		emitter.jump(2); //the dead code pass is not responsible for removing redundant jumps
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
		emitter.set_constant(0, 3);
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
		emitter.set_constant(0, 3);
	}
	p0::intermediate::function original_function(original_code, 0, 0);
	p0::intermediate::function::instruction_vector const living_code = p0::remove_dead_code(original_function.body());
	BOOST_CHECK(original_code == living_code);
}

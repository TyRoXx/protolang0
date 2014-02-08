#include "p0optimize/constant_folding.hpp"
#include "p0optimize/remove_no_ops.hpp"
#include "p0i/emitter.hpp"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(remove_no_ops_set_from_constant)
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
		emitter.set_constant(0, 3);
	}
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

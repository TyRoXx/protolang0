#include "p0optimize/inlining.hpp"
#include "p0i/emitter.hpp"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(inlining_empty_callee)
{
	p0::intermediate::function empty_function({}, 0, 0);
	std::vector<p0::intermediate::function> environment{empty_function};
	p0::intermediate::function::instruction_vector original_code;
	{
		p0::intermediate::emitter emitter(original_code);
		emitter.set_function(0, 0);
		emitter.call(0, 0);
		emitter.set_constant(0, 2);
	}
	p0::intermediate::function::instruction_vector const optimized = p0::inlining(original_code, environment);
	p0::intermediate::function::instruction_vector expected_code;
	{
		p0::intermediate::emitter emitter(expected_code);
		emitter.set_constant(0, 2);
	}
	BOOST_CHECK(expected_code == optimized);
}

BOOST_AUTO_TEST_CASE(inlining_trivially_returning_callee)
{
	p0::intermediate::function::instruction_vector callee_code;
	{
		p0::intermediate::emitter emitter(callee_code);
		emitter.set_constant(0, 12);
	}
	p0::intermediate::function callee(callee_code, 0, 0);
	std::vector<p0::intermediate::function> environment{callee};
	p0::intermediate::function::instruction_vector original_code;
	{
		p0::intermediate::emitter emitter(original_code);
		emitter.set_function(0, 0);
		emitter.call(0, 0);
	}
	p0::intermediate::function::instruction_vector const optimized = p0::inlining(original_code, environment);
	p0::intermediate::function::instruction_vector expected_code;
	{
		p0::intermediate::emitter emitter(expected_code);
		emitter.set_constant(0, 12);
	}
	BOOST_CHECK(expected_code == optimized);
}

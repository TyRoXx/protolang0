#include "p0optimize/analyzation.hpp"
#include "p0i/emitter.hpp"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(dissect_function_empty)
{
	p0::intermediate::function::instruction_vector const code;
	p0::function_section_graph const g = p0::dissect_function(code);
	BOOST_CHECK(g.sections.empty());
}

BOOST_AUTO_TEST_CASE(dissect_function_jump_if)
{
	p0::intermediate::function::instruction_vector code;
	{
		// return (arg0 ? 2 : 3)
		p0::intermediate::emitter emitter(code);
		//section 0
		emitter.set_constant(0, 2);
		emitter.jump_if(3, 1);
		//section 1
		emitter.set_constant(0, 3);
	}
	p0::function_section_graph const g = p0::dissect_function(code);
	BOOST_REQUIRE(g.sections.size() == 2);
	{
		p0::linear_section const &section = g.sections[0];
		BOOST_CHECK(section.code.begin() == code.data());
		BOOST_CHECK(section.code.end() == code.data() + 2);
		BOOST_CHECK((section.destinations == std::unordered_set<p0::section_id>{1, 2}));
		BOOST_CHECK((section.origins == std::unordered_set<p0::section_id>{p0::calling_section}));
	}
	{
		p0::linear_section const &section = g.sections[1];
		BOOST_CHECK(section.code.begin() == code.data() + 2);
		BOOST_CHECK(section.code.end() == code.data() + 3);
		BOOST_CHECK(section.destinations == std::unordered_set<p0::section_id>{2});
		BOOST_CHECK(section.origins == std::unordered_set<p0::section_id>{0});
	}
}

BOOST_AUTO_TEST_CASE(dissect_function_jump)
{
	p0::intermediate::function::instruction_vector code;
	{
		p0::intermediate::emitter emitter(code);
		//section 0
		emitter.set_constant(0, 2);
		emitter.jump(3);
		//section 1
		emitter.set_constant(0, 3);
	}
	p0::function_section_graph const g = p0::dissect_function(code);
	BOOST_REQUIRE(g.sections.size() == 2);
	{
		p0::linear_section const &section = g.sections[0];
		BOOST_CHECK(section.code.begin() == code.data());
		BOOST_CHECK(section.code.end() == code.data() + 2);
		BOOST_CHECK((section.destinations == std::unordered_set<p0::section_id>{2}));
		BOOST_CHECK(section.origins == std::unordered_set<p0::section_id>{p0::calling_section});
	}
	{
		p0::linear_section const &section = g.sections[1];
		BOOST_CHECK(section.code.begin() == code.data() + 2);
		BOOST_CHECK(section.code.end() == code.data() + 3);
		BOOST_CHECK(section.destinations == std::unordered_set<p0::section_id>{2});
		BOOST_CHECK(section.origins == std::unordered_set<p0::section_id>{});
	}
}

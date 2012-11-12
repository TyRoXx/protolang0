#include "p0i/ptree_load_unit.hpp"
#include "p0i/ptree_save_unit.hpp"
#include "p0i/unit.hpp"
#include <sstream>
#include <boost/property_tree/json_parser.hpp>
#include <boost/test/unit_test.hpp>


namespace
{
	void check_equal(
		p0::intermediate::instruction const &first,
		p0::intermediate::instruction const &second
		)
	{
		BOOST_REQUIRE_EQUAL(first.type(), second.type());

		for (size_t i = 0, c = p0::intermediate::get_instruction_info(
			first.type()).argument_count; i < c; ++i)
		{
			BOOST_CHECK_EQUAL(first.arguments()[i], second.arguments()[i]);
		}
	}

	void check_equal(
		p0::intermediate::function const &first,
		p0::intermediate::function const &second
		)
	{
		BOOST_CHECK_EQUAL(first.parameters(), second.parameters());
		BOOST_REQUIRE_EQUAL(first.body().size(), second.body().size());

		for (size_t i = 0, c = first.body().size(); i < c; ++i)
		{
			check_equal(
				first.body()[i],
				second.body()[i]
			);
		}
	}

	void check_equal(
		p0::intermediate::unit const &first,
		p0::intermediate::unit const &second
		)
	{
		BOOST_ASSERT(&first != &second);

		BOOST_CHECK_EQUAL(first.integer_width(), second.integer_width());
		BOOST_REQUIRE_EQUAL(first.functions().size(), second.functions().size());

		for (size_t f = 0, c = first.functions().size(); f < c; ++f)
		{
			check_equal(
				first.functions()[f],
				second.functions()[f]
			);
		}
	}
}

BOOST_AUTO_TEST_CASE(load_save_test)
{
	using namespace p0::intermediate;

	unit::function_vector functions;
	functions.push_back(function(
		function::instruction_vector(),
		0));

	functions.push_back(function(
		function::instruction_vector(),
		4));

	functions.push_back(function(
		function::instruction_vector(1, instruction(instruction_type::add, 0, 1)),
		2));

	unit::string_vector strings;
	size_t const integer_width = 64;

	unit const original_unit(functions, strings, integer_width);

	boost::property_tree::ptree original_tree;
	save_unit(
		original_tree,
		original_unit
		);

	std::ostringstream sink;
	boost::property_tree::write_json(
		sink,
		original_tree
		);

	auto const serialized = sink.str();
	BOOST_CHECK(!serialized.empty());

	std::istringstream source(serialized);
	boost::property_tree::ptree loaded_tree;
	boost::property_tree::read_json(
		source,
		loaded_tree
		);
	auto const loaded_unit = load_unit(
		loaded_tree
		);

	check_equal(original_unit, loaded_unit);
}

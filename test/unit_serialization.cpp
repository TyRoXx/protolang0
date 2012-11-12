#include "p0i/ptree_load_unit.hpp"
#include "p0i/ptree_save_unit.hpp"
#include "p0i/unit.hpp"
#include <sstream>
#include <boost/property_tree/json_parser.hpp>
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(load_save_test)
{
	using namespace p0::intermediate;

	unit::function_vector functions;
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
	std::istringstream source(serialized);
	boost::property_tree::ptree loaded_tree;
	boost::property_tree::read_json(
		source,
		loaded_tree
		);
	auto const loaded_unit = load_unit(
		loaded_tree
		);
}

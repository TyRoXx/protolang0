#include "p0rt/from_cpp.hpp"
#include "p0run/default_garbage_collector.hpp"
#include <boost/test/unit_test.hpp>
using namespace p0;

BOOST_AUTO_TEST_CASE(from_cpp_map_test)
{
	run::default_garbage_collector gc;
	run::interpreter interpreter(gc, nullptr);

	{
		std::map<int, std::string> cpp_map;
		BOOST_CHECK_EQUAL(rt::from_cpp(cpp_map, interpreter).type, run::value_type::object);

		cpp_map[1] = "hello";
		run::value const as_table = rt::from_cpp(cpp_map, interpreter);
		BOOST_REQUIRE(as_table.type == run::value_type::object);

		boost::optional<run::value> const value = as_table.obj->get_element(run::value(1));
		BOOST_REQUIRE(value);
		BOOST_CHECK_EQUAL(run::expect_string(*value), "hello");
	}

	{
		std::map<run::value, std::map<unsigned, char>> cpp_map;
		BOOST_CHECK_EQUAL(rt::from_cpp(cpp_map, interpreter).type, run::value_type::object);
	}
}

#include "p0rt/to_cpp.hpp"
#include "p0rt/insert.hpp"
#include "p0run/default_garbage_collector.hpp"
#include <boost/test/unit_test.hpp>
using namespace p0;

BOOST_AUTO_TEST_CASE(to_cpp_map_test)
{
	run::default_garbage_collector gc;
	run::table table;
	rt::inserter(table, gc)
	        .insert("a", 0)
	        .insert("b", 1)
	        .insert("c", 2)
	        ;

	std::map<std::string, run::integer> map;
	rt::to_cpp(map, run::value(table));

	BOOST_REQUIRE_EQUAL(map.size(), 3);
	BOOST_CHECK_EQUAL(map["a"], 0);
	BOOST_CHECK_EQUAL(map["b"], 1);
	BOOST_CHECK_EQUAL(map["c"], 2);
}

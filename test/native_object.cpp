#include "p0rt/native_object.hpp"
#include "p0run/interpreter.hpp"
#include "p0run/default_garbage_collector.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/foreach.hpp>
using namespace p0::run;
using namespace p0::rt;
using namespace p0;

namespace
{
	struct empty
	{
	};
}

BOOST_AUTO_TEST_CASE(empty_native_object_test)
{
	run::default_garbage_collector gc;
	run::interpreter interpreter_(gc, nullptr);
	native_object<empty> a, b;
	BOOST_CHECK(!a.call_method(run::value(), std::vector<run::value>(), interpreter_).is_initialized());
	BOOST_CHECK(a.equals(a));
	BOOST_CHECK(!a.equals(b));
	BOOST_CHECK(a.get_hash_code() == a.get_hash_code());
	BOOST_CHECK(!a.get_element(run::value()).is_initialized());
	BOOST_CHECK(!a.set_element(run::value(), run::value()));
	BOOST_CHECK(!a.call(std::vector<run::value>(), interpreter_).is_initialized());
}

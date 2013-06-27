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
	BOOST_CHECK(!a.bind(0, run::value()));
	BOOST_CHECK(!a.get_bound(0).is_initialized());
}

namespace
{
	struct has_methods
	{
		int returns_value()
		{
			return 0;
		}

		int returns_value(int)
		{
			return 0;
		}

		void returns_nothing()
		{
		}

		int const_method() const
		{
			return 0;
		}
	};

	using namespace native_object_policies;

	struct has_methods_policies
	        : do_not_mark
	        , native_methods<has_methods>
	        , not_callable
	        , print_object
	        , compare_object
	        , no_elements
	        , not_bindable
	{
	};
}

BOOST_AUTO_TEST_CASE(native_class_test)
{
	native_class<has_methods> cls;
	cls.add_method("returns_nothing", &has_methods::returns_nothing);
	cls.add_method("returns_value",
	               static_cast<int (has_methods::*)()>(&has_methods::returns_value));
	cls.add_method("returns_value",
	               static_cast<int (has_methods::*)(int)>(&has_methods::returns_value));
	cls.add_method("const_method", &has_methods::const_method);
}

BOOST_AUTO_TEST_CASE(native_object_methods_test)
{
	native_object<has_methods, has_methods_policies> obj;
//	obj.add_method("returns_value", &has_methods::returns_value);
}

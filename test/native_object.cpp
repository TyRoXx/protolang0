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
			return 123;
		}

		int returns_value(int)
		{
			return 456;
		}

		void returns_nothing()
		{
		}

		int const_method() const
		{
			return -3;
		}
	};

	using namespace native_object_policies;

	struct has_methods_policies
	        : do_not_mark
	        , native_methods_global<has_methods>
	        , not_callable
	        , print_object
	        , compare_object
	        , no_elements
	        , not_bindable
	{
	};

	void add_methods(native_class<has_methods> &cls)
	{
		cls.add_method("returns_nothing", &has_methods::returns_nothing);
		cls.add_method("returns_value",
		               static_cast<int (has_methods::*)()>(&has_methods::returns_value));
		cls.add_method("returns_value",
		               static_cast<int (has_methods::*)(int)>(&has_methods::returns_value));
		cls.add_method("const_method", &has_methods::const_method);
	}
}

BOOST_AUTO_TEST_CASE(native_class_test)
{
	native_class<has_methods> cls;
	add_methods(cls);
}

namespace p0
{
	namespace rt
	{
		namespace native_object_policies
		{
			template <>
			struct native_class_traits<has_methods>
			{
				static native_class<has_methods> const &description()
				{
					static native_class<has_methods> const instance = create_instance();
					return instance;
				}

			private:

				static native_class<has_methods> create_instance()
				{
					native_class<has_methods> instance;
					add_methods(instance);
					return instance;
				}
			};
		}
	}
}

BOOST_AUTO_TEST_CASE(native_object_methods_call_test)
{
	run::default_garbage_collector gc;
	run::interpreter interpreter_(gc, nullptr);
	run::string returns_nothing("returns_nothing");
	run::string returns_value("returns_value");
	native_object<has_methods, has_methods_policies> obj;

	BOOST_CHECK_EQUAL(run::value(), obj.call_method(
	            run::value(returns_nothing),
	            std::vector<run::value>(),
	            interpreter_));
	BOOST_CHECK_EQUAL(run::value(123), obj.call_method(
	            run::value(returns_value),
	            std::vector<run::value>(),
	            interpreter_));
	BOOST_CHECK_EQUAL(run::value(456), obj.call_method(
	            run::value(returns_value),
	            std::vector<run::value>(1, run::value(0)),
	            interpreter_));
}

namespace
{
	struct without_default_ctor
	{
		int a;
		std::string b;

		explicit without_default_ctor(int a, std::string b)
		    : a(a)
		    , b(std::move(b))
		{
		}

		int add_a(int other) const
		{
			return a + other;
		}
	};

	struct without_default_ctor_policies
	        : do_not_mark
	        , native_methods_from_ctor<without_default_ctor>
	        , not_callable
	        , print_object
	        , compare_object
	        , no_elements
	        , not_bindable
	{
		explicit without_default_ctor_policies(native_class<without_default_ctor> const &description)
		    : native_methods_from_ctor<without_default_ctor>(description)
		{
		}
	};
}

BOOST_AUTO_TEST_CASE(native_object_policy_args_test)
{
	run::default_garbage_collector gc;
	run::interpreter interpreter_(gc, nullptr);

	native_class<without_default_ctor> wd_class;
	wd_class.add_method("add_a", &without_default_ctor::add_a);

	native_object<without_default_ctor, without_default_ctor_policies> obj(
	            policy_arg(), std::ref(wd_class),
	            123,
	            "hello");

	auto &value = obj.value();
	BOOST_CHECK_EQUAL(value.a, 123);
	BOOST_CHECK_EQUAL(value.b, "hello");

	run::string add_a("add_a");
	BOOST_CHECK_EQUAL(run::value(125),
	                  obj.call_method(run::value(add_a), std::vector<run::value>(1, run::value(2)), interpreter_));
}

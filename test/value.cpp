#include "p0run/value.hpp"
#include "p0run/object.hpp"
#include "p0i/unit.hpp"
#include "p0i/function_ref.hpp"
#include <boost/test/unit_test.hpp>
using namespace p0::run;
using namespace std::rel_ops;


namespace
{
	struct dummy_object : object
	{
		virtual void mark_recursively() override
		{
		}

		virtual boost::optional<value> get_element(value const & /*key*/) const override
		{
			return boost::optional<value>();
		}

		virtual bool set_element(value const & /*key*/, value const & /*value*/) override
		{
			return false;
		}
	};

	void check_equal_impl(value const &left, value const &right)
	{
		BOOST_CHECK(left == right);
		BOOST_CHECK(left <= right);
		BOOST_CHECK(left >= right);
		BOOST_CHECK(!(left < right));
		BOOST_CHECK(!(left > right));
		BOOST_CHECK(compare(left, right) == comparison_result::equal);
		BOOST_CHECK(compare(left, right) != comparison_result::less);
		BOOST_CHECK(compare(left, right) != comparison_result::greater);
	}

	void check_equal(value const &left, value const &right)
	{
		check_equal_impl(left, right);
		check_equal_impl(right, left);
	}
}

BOOST_AUTO_TEST_CASE(equal_value_test)
{
	check_equal(value(), value());
	check_equal(value(0), value(0));
	check_equal(value(123), value(123));

	p0::intermediate::function_ref function_a;
	check_equal(value(function_a), value(function_a));

	dummy_object object_a;
	check_equal(value(object_a), value(object_a));
}


namespace
{
	void check_unequal_impl(value const &left, value const &right)
	{
		BOOST_CHECK(left != right);
		BOOST_CHECK(!(left == right));
		BOOST_CHECK(compare(left, right) != comparison_result::equal);
	}

	void check_unequal(value const &left, value const &right)
	{
		check_unequal_impl(left, right);
		check_unequal_impl(right, left);
	}
}

BOOST_AUTO_TEST_CASE(unequal_value_test)
{
	check_unequal(value(), value(0));
	check_unequal(value(), value(123));
	check_unequal(value(-1), value(1));
	check_unequal(value(0), value(1));

	p0::intermediate::unit const program(
				(p0::intermediate::unit::function_vector()),
				p0::intermediate::unit::string_vector());
	p0::intermediate::function const function_a, function_b;
	p0::intermediate::function_ref const
			ref_a(program, function_a),
			ref_b(program, function_b);

	check_unequal(value(ref_a), value(ref_b));
	check_unequal(value(ref_a), value());
	check_unequal(value(ref_a), value(123));

	dummy_object object_a, object_b;
	check_unequal(value(object_a), value(object_b));
	check_unequal(value(object_a), value());
	check_unequal(value(object_a), value(123));
	check_unequal(value(object_a), value(ref_a));
}


namespace
{
	void check_less(value const &left, value const &right)
	{
		BOOST_CHECK(left < right);
		BOOST_CHECK(left != right);
		BOOST_CHECK(compare(left, right) == comparison_result::less);
	}
}

BOOST_AUTO_TEST_CASE(less_value_test)
{
	check_less(value(), value(0));
	check_less(value(0), value(1));
	check_less(value(-1), value(1));
	check_less(value(), value(-1));

	p0::intermediate::unit const program(
				(p0::intermediate::unit::function_vector()),
				p0::intermediate::unit::string_vector());
	p0::intermediate::function const function_a, function_b;
	p0::intermediate::function_ref const
			ref_a(program, function_a),
			ref_b(program, function_b);
	check_less(value(), value(ref_b));
	check_less(value(ref_a), value(0));
	check_less(value(ref_b), value(0));

	if (&function_a < &function_b)
	{
		check_less(value(ref_a), value(ref_b));
	}
	else
	{
		check_less(value(ref_b), value(ref_a));
	}

	dummy_object object_a, object_b;
	check_less(value(), value(object_a));
	check_less(value(object_a), value(123));
	check_less(value(ref_a), value(object_a));

	if (&object_a < &object_b)
	{
		check_less(value(object_a), value(object_b));
	}
	else
	{
		check_less(value(object_b), value(object_a));
	}
}

BOOST_AUTO_TEST_CASE(value_is_null_test)
{
	p0::intermediate::function_ref function;
	dummy_object object;

	BOOST_CHECK(is_null(value()));
	BOOST_CHECK(!is_null(value(static_cast<integer>(123))));
	BOOST_CHECK(!is_null(value(function)));
	BOOST_CHECK(!is_null(value(object)));
}

#include "p0run/value.hpp"
#include "p0i/function.hpp"
#include <boost/test/unit_test.hpp>
using namespace p0::run;
using namespace std::rel_ops;


namespace
{
	void check_equal(value const &left, value const &right)
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
}

BOOST_AUTO_TEST_CASE(equal_value_test)
{
	check_equal(value(), value());
	check_equal(value(0), value(0));
	check_equal(value(123), value(123));

	p0::intermediate::function function_a;
	check_equal(value(function_a), value(function_a));
}


namespace
{
	void check_unequal(value const &left, value const &right)
	{
		BOOST_CHECK(left != right);
		BOOST_CHECK(!(left == right));
		BOOST_CHECK(compare(left, right) != comparison_result::equal);
	}
}

BOOST_AUTO_TEST_CASE(unequal_value_test)
{
	check_unequal(value(), value(0));
	check_unequal(value(), value(123));
	check_unequal(value(-1), value(1));
	check_unequal(value(0), value(1));

	p0::intermediate::function function_a, function_b;
	check_unequal(value(function_a), value(function_b));
	check_unequal(value(function_a), value());
	check_unequal(value(function_a), value(123));
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

	p0::intermediate::function function_a, function_b;
	check_less(value(), value(function_b));
	check_less(value(function_a), value(0));
	check_less(value(function_b), value(0));

	if (&function_a < &function_b)
	{
		check_less(value(function_a), value(function_b));
	}
	else
	{
		check_less(value(function_b), value(function_a));
	}
}

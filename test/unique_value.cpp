#include "p0common/unique_value.hpp"
#include <boost/test/unit_test.hpp>

namespace
{
	template <class Value>
	struct null_ownership_policy
	{
		void acquire(Value &)
		{
		}

		void release(Value &)
		{
		}
	};

	template <class Value>
	struct counting_ownership_policy
	{
		std::size_t *references;

		counting_ownership_policy(std::size_t *references)
			: references(references)
		{
		}

		void acquire(Value &)
		{
			BOOST_REQUIRE(*references < std::numeric_limits<std::size_t>::max());
			++(*references);
		}

		void release(Value &)
		{
			BOOST_REQUIRE(*references > 0);
			--(*references);
		}
	};
}

BOOST_AUTO_TEST_CASE(unique_value_trivial)
{
	typedef p0::unique_value<int, null_ownership_policy<int>> value;

	value a;
	value b(123);
	value c(456, null_ownership_policy<int>());

	BOOST_CHECK(*b == 123);
	BOOST_CHECK(*c == 456);

	a = std::move(b);

	BOOST_CHECK(*a == 123);
}

BOOST_AUTO_TEST_CASE(unique_value_counting)
{
	typedef p0::unique_value<int, counting_ownership_policy<int>> value;

	std::size_t counter = 0;
	{
		value a(123, counting_ownership_policy<int>(&counter));
		BOOST_CHECK(counter == 1);

		auto b = std::move(a);
		BOOST_CHECK(counter == 1);
	}

	BOOST_CHECK(counter == 0);
}

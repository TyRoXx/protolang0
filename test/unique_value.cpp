#include "p0common/unique_value.hpp"
#include <boost/test/unit_test.hpp>

namespace
{
	template <class Value>
	struct null_ownership_policy
	{
		typedef Value storage_type;

		void acquire(Value &)
		{
		}

		void release(Value &)
		{
		}

		Value const &get_reference(storage_type const &storage) const
		{
			return storage;
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

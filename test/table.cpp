#include "p0run/table.hpp"
#include "p0run/default_garbage_collector.hpp"
#include "p0run/raw_storage.hpp"
#include <boost/test/unit_test.hpp>
#include <array>
using namespace p0::run;

namespace
{
	struct test_object : object
	{
	private:

		virtual void mark_recursively() PROTOLANG0_OVERRIDE
		{
		}
	};

	value make_test_object(garbage_collector &gc)
	{
		return value(construct_object<test_object>(gc));
	}
}

BOOST_AUTO_TEST_CASE(table_mark)
{
	default_garbage_collector gc;
	table::elements elements;
	std::array<value, 40> objects;
	for (std::size_t i = 0; i < objects.size() / 2; ++i)
	{
		elements[objects[i * 2] = make_test_object(gc)] =
			(objects[i * 2 + 1] = make_test_object(gc));
	}

	auto const is_marked_object = [](value const &element)
	{
		return (element.type == value_type::object) &&
			   (element.obj->is_marked());
	};

	table table((elements));
	BOOST_CHECK(std::none_of(begin(objects), end(objects), is_marked_object));

	table.mark();
	BOOST_CHECK(std::all_of(begin(objects), end(objects), is_marked_object));
}

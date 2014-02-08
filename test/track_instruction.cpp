#include "p0optimize/local_constant_tracker.hpp"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(track_instruction_set_from_constant)
{
	p0::local_constant_tracker tracker;
	p0::track_instruction(tracker, p0::intermediate::instruction(p0::intermediate::instruction_type::set_from_constant, 0, 2));
	BOOST_CHECK(2L == tracker.find_current_value(0));
	BOOST_CHECK(!tracker.find_current_value(1));
}

BOOST_AUTO_TEST_CASE(track_instruction_copy_positive)
{
	p0::local_constant_tracker tracker;
	tracker.update_value(1, 3);
	p0::track_instruction(tracker, p0::intermediate::instruction(p0::intermediate::instruction_type::copy, 0, 1));
	BOOST_CHECK(3L == tracker.find_current_value(0));
	BOOST_CHECK(3L == tracker.find_current_value(1));
}

BOOST_AUTO_TEST_CASE(track_instruction_copy_negative)
{
	p0::local_constant_tracker tracker;
	tracker.update_value(0, 3);
	p0::track_instruction(tracker, p0::intermediate::instruction(p0::intermediate::instruction_type::copy, 0, 1));
	BOOST_CHECK(!tracker.find_current_value(0));
	BOOST_CHECK(!tracker.find_current_value(1));
}

BOOST_AUTO_TEST_CASE(track_instruction_add_positive)
{
	p0::local_constant_tracker tracker;
	tracker.update_value(0, 2);
	tracker.update_value(1, 3);
	p0::track_instruction(tracker, p0::intermediate::instruction(p0::intermediate::instruction_type::add, 0, 1));
	BOOST_CHECK(5L == tracker.find_current_value(0));
	BOOST_CHECK(3L == tracker.find_current_value(1));
}

BOOST_AUTO_TEST_CASE(track_instruction_add_negative)
{
	p0::local_constant_tracker tracker;
	tracker.update_value(0, 3);
	p0::track_instruction(tracker, p0::intermediate::instruction(p0::intermediate::instruction_type::add, 0, 1));
	BOOST_CHECK(!tracker.find_current_value(0));
	BOOST_CHECK(!tracker.find_current_value(1));
}

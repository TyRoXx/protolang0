#include "p0run/default_garbage_collector.hpp"
#include "p0run/object.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/foreach.hpp>
#include <algorithm>
using namespace p0::run;


BOOST_AUTO_TEST_CASE(default_gc_test)
{
	typedef int vector_safe_bool;

	struct gc_tester PROTOLANG0_FINAL_CLASS : p0::run::object
	{
		explicit gc_tester(vector_safe_bool &is_alive)
			: m_is_alive(is_alive)
		{
			m_is_alive = true;
		}

		virtual ~gc_tester()
		{
			m_is_alive = false;
		}

	private:

		vector_safe_bool &m_is_alive;

		virtual void mark_recursively() PROTOLANG0_FINAL_METHOD
		{
		}
	};

	p0::run::default_garbage_collector gc;

	std::vector<vector_safe_bool> alive_status(64);
	std::vector<p0::run::object *> test_objects;

	BOOST_FOREACH (auto &is_alive, alive_status)
	{
		test_objects.push_back(
					&construct_object<gc_tester>(gc, is_alive));
	}

	//all created objects are expected to be alive before garbage collection
	BOOST_CHECK(std::all_of(alive_status.begin(),
							alive_status.end(),
							[](bool a) { return a; }));

	//not strictly required here, but called in practice before marking
	gc.unmark();

	//mark the first half of the objects as required to be alive
	std::for_each(test_objects.begin(),
				  test_objects.begin() + test_objects.size() / 2,
				  [](p0::run::object *object)
	{
		object->mark();
	});

	//let the gc destroy unneeded objects
	gc.sweep(p0::run::sweep_mode::full);

	//the marked half is expected to be still alive
	BOOST_CHECK(std::all_of(alive_status.begin(),
							alive_status.begin() + alive_status.size() / 2,
							[](bool a) { return a; }));

	//the half not marked is expected to not be alive after sweep
	BOOST_CHECK(std::all_of(alive_status.begin() + alive_status.size() / 2,
							alive_status.end(),
							[](bool a) { return !a; }));
}

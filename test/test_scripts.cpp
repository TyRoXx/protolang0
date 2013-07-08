#include "p0run/interpreter.hpp"
#include "p0run/default_garbage_collector.hpp"
#include "p0run/runtime_error_exception.hpp"
#include "p0compile/compile_unit.hpp"
#include "p0rt/expose.hpp"
#include "p0rt/module_cache.hpp"
#include "p0rt/std_module.hpp"
#ifdef PROTOLANG0_WITH_TEMPEST
#	include "p0tempest/tempest_module.hpp"
#endif
#include <boost/test/unit_test.hpp>
#include <boost/filesystem/operations.hpp>
#include <iostream>


namespace
{
	struct Tester
	{
		explicit Tester(boost::filesystem::path test_scripts)
			: m_test_scripts(std::move(test_scripts))
		{
		}

		void run_test_script(const std::string &file_name) const
		{
			const auto full_file_name = m_test_scripts / file_name;
			const auto script = p0::compile_unit_from_file(full_file_name.string());

			p0::run::default_garbage_collector gc;
			p0::rt::module_cache modules;
			modules.add_module("std", p0::rt::lazy_module(p0::rt::register_standard_module));
			modules.add_module(p0::tempest::default_module_name,
			                   p0::rt::lazy_module(p0::tempest::register_tempest_module));
			p0::run::interpreter interpreter(gc, p0::rt::make_import(modules));

			BOOST_REQUIRE(!script.functions().empty());
			const auto main = interpreter.call(
				p0::intermediate::function_ref(script, script.functions().front()),
				{});

			std::size_t assertionIndex = 0;

			std::vector<p0::run::value> arguments;
			arguments.push_back(p0::rt::expose_fn(
				gc,
				[&assertionIndex, &file_name]
				(const std::vector<p0::run::value> &arguments)
			{
				++assertionIndex;
				BOOST_REQUIRE(arguments.size() == 1);
				auto const condition = arguments.front();
				if (!p0::run::to_boolean(condition))
				{
					std::cerr << file_name
							  << ": The " << assertionIndex << ". assert failed\n";
					BOOST_CHECK(nullptr == "Assertion failed");
				}
				return p0::run::value{};
			}));

			BOOST_REQUIRE(main.type == p0::run::value_type::function_ptr);
			try
			{
				interpreter.call(*main.function_ptr, arguments);
			}
			catch (p0::run::runtime_error_exception const &ex)
			{
				std::cerr << ex.error() << '\n';
				throw;
			}
		}

	private:

		const boost::filesystem::path m_test_scripts;
	};

	void run_test_scripts(const std::string &testScriptsPath)
	{
		Tester t((testScriptsPath));
		t.run_test_script("array.p0");
		t.run_test_script("empty.p0");
		t.run_test_script("closure.p0");
		t.run_test_script("integer.p0");
		t.run_test_script("table.p0");
		t.run_test_script("null.p0");
#ifdef PROTOLANG0_WITH_TEMPEST
		t.run_test_script("tempest.p0");
#endif
	}
}

BOOST_AUTO_TEST_CASE(test_scripts)
{
	const std::string testScriptsPath = "./test-scripts";
	if (boost::filesystem::exists(testScriptsPath))
	{
		run_test_scripts(testScriptsPath);
	}
	else
	{
		std::cerr
			<< "Put the 'test-scripts' directory into working directory "
			   "(e.g. with a soft link) to enable execution of the contained "
			   "scripts.\n";
	}
}

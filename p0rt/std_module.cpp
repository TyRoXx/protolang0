#include "std_module.hpp"
#include "insert.hpp"
#include "p0run/garbage_collector.hpp"
#include <boost/foreach.hpp>
#include <vector>
#include <sstream>


namespace p0
{
	namespace rt
	{
		namespace
		{
			run::value std_print_string(std::vector<run::value> const &arguments)
			{
				auto &out = std::cout;
				BOOST_FOREACH (auto &argument, arguments)
				{
					out << argument;
				}
				return run::value();
			}

			run::value std_read_line(
				run::garbage_collector &gc,
				std::vector<run::value> const & /*arguments*/)
			{
				auto &in = std::cin;
				std::string line;
				getline(in, line);
				return rt::expose(gc, std::move(line));
			}

			run::value std_assert(std::vector<run::value> const &arguments)
			{
				if (arguments.empty() ||
					!run::to_boolean(arguments.front()))
				{
					//TODO better handling
					throw std::runtime_error("Assertion failed");
				}
				return run::value();
			}

			run::value std_to_string(run::garbage_collector &gc,
									 std::vector<run::value> const &arguments)
			{
				std::ostringstream result;
				BOOST_FOREACH (auto &argument, arguments)
				{
					result << argument;
				}
				return rt::expose(gc, result.str());
			}
		}

		run::value register_standard_module(run::garbage_collector &gc)
		{
			auto &module = run::construct_object<run::table>(gc);
			rt::inserter(module, gc)
				.insert_fn("print", &std_print_string)
				.insert_fn("read_line", std::bind(std_read_line, std::ref(gc), std::placeholders::_1))
				.insert_fn("assert", &std_assert)
				.insert_fn("to_string", std::bind(std_to_string, std::ref(gc), std::placeholders::_1))
				;
			return run::value(module);
		}
	}
}

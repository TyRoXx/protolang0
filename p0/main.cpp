#include "p0i/unit.hpp"
#include "p0compile/compile_unit.hpp"
#include "p0run/interpreter.hpp"
#include "p0run/table.hpp"
#include "p0rt/insert.hpp"
#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>
#include <boost/foreach.hpp>


namespace p0
{
	namespace
	{
		intermediate::unit load_unit(std::string const &file_name)
		{
			std::ifstream unit_file(file_name);
			if (!unit_file)
			{
				throw std::runtime_error(
					"Could not open unit file " + file_name
					);
			}

			throw std::runtime_error("Not implemented");
		}

		run::value std_print_string(std::vector<run::value> const &arguments)
		{
			auto &out = std::cout;
			BOOST_FOREACH (auto &argument, arguments)
			{
				out << argument;
			}
			return run::value();
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

		run::value std_to_string(run::interpreter &interpreter,
								 std::vector<run::value> const &arguments)
		{
			std::ostringstream result;
			BOOST_FOREACH (auto &argument, arguments)
			{
				result << argument;
			}
			return rt::expose(interpreter, result.str());
		}

		std::unique_ptr<run::object> load_standard_module(
				run::interpreter &interpreter,
				std::string const &name)
		{
			std::unique_ptr<run::object> module;
			if (name == "std")
			{
				module.reset(new run::table);
				rt::inserter(*module, interpreter)
					.insert_fn("print", &std_print_string)
					.insert_fn("assert", &std_assert)
					.insert_fn("to_string", std::bind(std_to_string, std::ref(interpreter), std::placeholders::_1))
					;
			}
			return module;
		}

		void execute(intermediate::unit const &program,
					 intermediate::function const &entry_point)
		{
			run::interpreter interpreter(program, load_standard_module);
			std::vector<run::value> arguments;
			interpreter.call(entry_point, arguments);
		}
	}
}

int main(int argc, char **argv)
{
	namespace po = boost::program_options;

	std::string file_name;
	std::size_t entry_point_id = 0;

	po::options_description desc("");
	desc.add_options()
		("help", "produce help message")
		("file,f", po::value<std::string>(&file_name), "file name")
		("compile,c", "")
		("entry,e", po::value<std::size_t>(&entry_point_id), "entry function id")
		;

	po::positional_options_description p;
	p.add("file", 1);

	po::variables_map vm;
	try
	{
		po::store(
			po::command_line_parser(argc, argv).options(desc).positional(p).run(),
			vm);
		po::notify(vm);
	}
	catch (po::error const &e)
	{
		std::cerr << e.what() << "\n";
		return 1;
	}

	if ((argc == 1) ||
		vm.count("help"))
	{
		std::cerr << desc << "\n";
		return 0;
	}

	try
	{
		bool const do_compile_file = (vm.count("compile") > 0);
		auto const unit = do_compile_file
				? p0::compile_unit_from_file(file_name)
				: p0::load_unit(file_name);

		p0::intermediate::function const *entry_point = 0;
		if (entry_point_id < unit.functions().size())
		{
			entry_point = &unit.functions()[entry_point_id];
		}
		else
		{
			std::cerr << "Entry function " << entry_point_id
					  << " does not exist\n";
			return 1;
		}

		p0::execute(unit, *entry_point);
	}
	catch (std::exception const &ex)
	{
		std::cerr << ex.what() << "\n";
		return 1;
	}
}

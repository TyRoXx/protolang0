#include "p0i/unit.hpp"
#include "p0compile/compile_unit.hpp"
#include "p0run/interpreter.hpp"
#include "p0run/table.hpp"
#include "p0run/string.hpp"
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

		template <class F>
		struct native_function : run::object
		{
			template <class G>
			explicit native_function(G &&functor)
				: m_functor(std::forward<G>(functor))
			{
			}

			virtual boost::optional<run::value> call(
					std::vector<run::value> const &arguments) const
			{
				return m_functor(arguments);
			}

		private:

			F const m_functor;

			virtual void mark_recursively() override
			{
			}
		};

		template <class F>
		std::unique_ptr<run::object> make_function(F &&functor)
		{
			return std::unique_ptr<run::object>(
						new native_function<F>(std::forward<F>(functor)));
		}

		run::value print_string(std::vector<run::value> const &arguments)
		{
			auto &out = std::cout;
			BOOST_FOREACH (auto &argument, arguments)
			{
				out << argument;
			}
			return run::value();
		}

		template <class F>
		run::object &register_function(run::interpreter &interpreter, F &&functor)
		{
			return interpreter.register_object(
						make_function(std::forward<F>(functor)));
		}

		run::object &register_string(run::interpreter &interpreter, std::string content)
		{
			return interpreter.register_object(
						std::unique_ptr<run::object>(new run::string(std::move(content))));
		}

		void insert(run::object &table,
					run::interpreter &interpreter,
					std::string key,
					run::value const &element)
		{
			if (!table.set_element(
						run::value(register_string(interpreter, std::move(key))),
						element
						))
			{
				//TODO better error handling
				throw std::runtime_error("Cannot set element in table");
			}
		}

		struct standard_library : run::table
		{
			explicit standard_library(run::interpreter &interpreter)
			{
				insert(*this, interpreter,
					   "print", run::value(register_function(interpreter, print_string)));
			}
		};

		std::unique_ptr<run::object> load_standard_module(
				run::interpreter &interpreter,
				std::string const &name)
		{
			std::unique_ptr<run::object> module;
			if (name == "std")
			{
				module.reset(new standard_library(interpreter));
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
	catch (const boost::program_options::error &e)
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

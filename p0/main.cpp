#include "p0i/unit.hpp"
#include "p0compile/compile_unit.hpp"
#include "p0run/interpreter.hpp"
#include "p0run/default_garbage_collector.hpp"
#include "p0run/table.hpp"
#include "p0run/runtime_error.hpp"
#include "p0rt/std_module.hpp"
#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>
#include <boost/foreach.hpp>


namespace p0
{
	namespace
	{
		intermediate::unit load_intermediate_code(std::string const &file_name)
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

		struct lazy_module
		{
			typedef std::function<run::value (run::garbage_collector &)> loader;

			run::value cached;
			loader load;

			lazy_module()
			{
			}

			explicit lazy_module(run::value const &already_usable)
				: cached(already_usable)
			{
			}

			explicit lazy_module(loader delayed)
				: load(std::move(delayed))
			{
			}
		};

		typedef std::function<void (std::string, lazy_module)> module_consumer;
		typedef std::function<bool (std::string const &, module_consumer const &)>
			module_file_decoder;

		struct module_loader PROTOLANG0_FINAL_CLASS
		{
			void register_file_decoder(module_file_decoder decoder)
			{
				m_decoders.push_back(std::move(decoder));
			}

			bool initialize_modules_from_file(
				std::string const &file_name,
				module_consumer const &handle_module)
			{
				for (auto f = begin(m_decoders); f != end(m_decoders); ++f)
				{
					auto const &decode = *f;
					if (decode(file_name, handle_module))
					{
						return true;
					}
				}
				return false;
			}

		private:

			std::vector<module_file_decoder> m_decoders;
		};

		struct module_storage PROTOLANG0_FINAL_CLASS
		{
			void add_module(
				std::string name,
				lazy_module module)
			{
				if (m_modules.find(name) != m_modules.end())
				{
					throw std::runtime_error(
						"The module name '" + name + "' is already in use");
				}

				m_modules.insert(std::make_pair(std::move(name), std::move(module)));
			}

			run::value get_module(
				run::garbage_collector &gc,
				std::string const &name)
			{
				auto const m = m_modules.find(name);
				if (m == m_modules.end())
				{
					return run::value();
				}

				auto &module = m->second;
				if (module.cached == run::value())
				{
					module.cached = module.load(gc);
				}

				return module.cached;
			}

		private:

			typedef std::map<std::string, lazy_module> modules_by_name;


			modules_by_name m_modules;
		};
	}
}

int main(int argc, char **argv)
{
	namespace po = boost::program_options;

	std::string file_name;
	std::size_t entry_point_id = 0;
	std::vector<std::string> external_module_file_names;

	po::options_description desc("");
	desc.add_options()
		("help", "produce help message")
		("file,f", po::value(&file_name), "file name")
		("compile,c", "")
		("entry,e", po::value(&entry_point_id), "entry function id")
		("library,l", po::value(&external_module_file_names), "")
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
		auto const program = do_compile_file
				? p0::compile_unit_from_file(file_name)
				: p0::load_intermediate_code(file_name)
				;

		p0::intermediate::function const *entry_point = 0;
		if (entry_point_id < program.functions().size())
		{
			entry_point = &program.functions()[entry_point_id];
		}
		else
		{
			std::cerr << "Entry function " << entry_point_id
					  << " does not exist\n";
			return 1;
		}

		p0::module_storage module_storage;
		p0::module_loader module_loader;

		//TODO: register file decoders to module_loader

		BOOST_FOREACH (auto const &file_name, external_module_file_names)
		{
			module_loader.initialize_modules_from_file(
				file_name,
				[&module_storage](std::string name, p0::lazy_module module)
			{
				module_storage.add_module(
					std::move(name),
					std::move(module));
			});
		}

		p0::run::default_garbage_collector gc;

		auto const get_module =
			[&module_storage, &gc](p0::run::interpreter &,
								   std::string const &name) -> p0::run::value
		{
			return module_storage.get_module(gc, name);
		};

		p0::run::interpreter interpreter(gc, get_module);
		module_storage.add_module("std",
								  p0::lazy_module(p0::rt::register_standard_module(gc)));

		std::vector<p0::run::value> arguments;
		try
		{
			interpreter.call(
						p0::intermediate::function_ref(program, *entry_point),
						arguments);
		}
		catch (p0::run::runtime_error const &error)
		{
			auto const function_id =
					std::distance(&error.function().function(),
								  &error.function().origin().functions().front());

			std::cerr
					<< "Error "
					<< error.type()
					<< " (" << p0::run::runtime_error_code::to_string(error.type()) << ")\n"
					<< "Function id " << function_id << '\n'
					<< "Instruction index " << error.instruction() << '\n';
			return 1;
		}
	}
	catch (std::exception const &ex)
	{
		std::cerr << ex.what() << "\n";
		return 1;
	}
}

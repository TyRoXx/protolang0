#include "p0i/unit.hpp"
#include "p0i/save_unit.hpp"
#include "p0i/unit_info.hpp"
#include "p0compile/compile_unit.hpp"
#include "p0run/interpreter.hpp"
#include "p0run/default_garbage_collector.hpp"
#include "p0run/table.hpp"
#include "p0run/runtime_error.hpp"
#include "p0run/runtime_error_exception.hpp"
#include "p0rt/module_cache.hpp"
#include "p0rt/std_module.hpp"
#ifdef PROTOLANG0_WITH_TEMPEST
#	include "p0tempest/tempest_module.hpp"
#endif
#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>


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
	}
}

int main(int argc, char **argv)
{
	namespace po = boost::program_options;

	auto &err = std::cerr;

	std::string file_name;
	std::size_t entry_point_id = 0;
	std::vector<std::string> external_module_file_names;
	std::string intermediate_output_file;

	po::options_description desc("");
	desc.add_options()
		("help", "produce help message")
		("file,f", po::value(&file_name), "file name")
		("compile,c", "")
		("entry,e", po::value(&entry_point_id), "entry function id")
		("library,l", po::value(&external_module_file_names), "")
		("intermediate,i", po::value(&intermediate_output_file), "")
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
		err << e.what() << "\n";
		return 1;
	}

	if ((argc == 1) ||
		vm.count("help"))
	{
		err << desc << "\n";
		return 0;
	}

	try
	{
		bool const do_compile_file = (vm.count("compile") > 0);
		auto const program = do_compile_file
				? p0::compile_unit_from_file(file_name)
				: p0::load_intermediate_code(file_name)
				;

		if (!intermediate_output_file.empty())
		{
			std::ofstream out_file(intermediate_output_file);
			if (!out_file)
			{
				err << "Could not open intermediate output file "
					<< intermediate_output_file << '\n';
				return 1;
			}
			p0::intermediate::save_unit(out_file, program);
		}

		p0::intermediate::function const *entry_point = 0;
		if (entry_point_id < program.functions().size())
		{
			entry_point = &program.functions()[entry_point_id];
		}
		else
		{
			err << "Entry function " << entry_point_id
				<< " does not exist\n";
			return 1;
		}

		p0::rt::module_cache module_storage;
		p0::rt::module_loader module_loader;

		//TODO: register file decoders to module_loader

		BOOST_FOREACH (auto const &file_name, external_module_file_names)
		{
			module_loader.initialize_modules_from_file(
				file_name,
				[&module_storage](std::string name, p0::rt::lazy_module module)
			{
				module_storage.add_module(
					std::move(name),
					std::move(module));
			});
		}

		p0::run::default_garbage_collector gc;
		p0::run::interpreter interpreter(gc, p0::rt::make_import(module_storage));
		module_storage.add_module("std",
								  p0::rt::lazy_module(p0::rt::register_standard_module(interpreter)));
#ifdef PROTOLANG0_WITH_TEMPEST
		module_storage.add_module(p0::tempest::default_module_name,
		                          p0::rt::lazy_module(boost::bind(p0::tempest::register_tempest_module, boost::ref(interpreter))));
#endif

		std::vector<p0::run::value> arguments;
		try
		{
			interpreter.call(
						p0::intermediate::function_ref(program, *entry_point),
						arguments);
		}
		catch (p0::run::runtime_error_exception const &ex)
		{
			auto &error = ex.error();
			auto &program = error.function().origin();

			//TODO: generate/load debug info
			p0::intermediate::unit_info * const debug_info = nullptr;

			auto const function_id = static_cast<size_t>(
					std::distance(
						&program.functions().front(),
						&error.function().function()));

			err << "Error ";
			if (debug_info)
			{
				err << "In unit " << debug_info->name << '\n';
			}
			err	<< error.type()
				<< " (" << p0::run::runtime_error_code::to_string(error.type()) << ")\n";

			err << "Function id " << function_id << '\n';
			if (debug_info)
			{
				err << "Function name "
					<< debug_info->functions[function_id].name << '\n';
			}

			err << "Instruction index " << error.instruction() << '\n';
			if (debug_info)
			{
				auto const position =
					debug_info->functions[function_id].instructions[error.instruction()];
				err << "Line " << (1 + position.row)
					<< ", column " << (1 + position.column) << '\n';
			}
			return 1;
		}
	}
	catch (std::exception const &ex)
	{
		err << ex.what() << "\n";
		return 1;
	}
}

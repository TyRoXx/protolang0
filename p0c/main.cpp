#include "p0compile/compiler.hpp"
#include "p0compile/compiler_error.hpp"
#include "p0compile/pretty_print_error.hpp"
#include "p0compile/compile_unit.hpp"
#include "p0i/save_unit.hpp"
#include "p0optimize/fold_constants.hpp"
#include "p0optimize/remove_dead_code.hpp"
#include "p0optimize/remove_no_ops.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <boost/program_options.hpp>
#include <boost/foreach.hpp>
using namespace std;

namespace p0
{
	intermediate::function optimize_function(intermediate::function const &original)
	{
		auto a = p0::fold_constants(original.body());
		auto b = p0::remove_no_ops(a);
		auto c = p0::remove_dead_code(b);
		return intermediate::function(std::move(c), original.parameters(), original.bound_variables());
	}

	intermediate::unit optimize(intermediate::unit const &original)
	{
		intermediate::unit::function_vector functions;
		BOOST_FOREACH (auto const &original_function, original.functions())
		{
			functions.push_back(optimize_function(original_function));
		}
		return intermediate::unit(std::move(functions), original.strings());
	}
}

int main(int argc, char **argv)
{
	namespace po = boost::program_options;

	std::string source_file_name;
	std::string output_file_name = "out.p0i";

	bool with_jump_labels = true;

	po::options_description desc("");
	desc.add_options()
		("help", "produce help message")
		("source,s", po::value<std::string>(&source_file_name), "source file name")
		("out,o", po::value<std::string>(&output_file_name), "output file name")
		("optimize,O", "do some simple optimization on the generated intermediate code")
		("labels", po::value(&with_jump_labels), "prepend instructions with their jump addresses in the output")
		;

	po::positional_options_description p;
	p.add("source", 1);
	p.add("out", 1);

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

	bool do_optimize = (vm.count("optimize") > 0);

	try
	{
		auto compiled_unit = p0::compile_unit_from_file(source_file_name);

		if (do_optimize)
		{
			compiled_unit = p0::optimize(compiled_unit);
		}

		std::ofstream target_file(
			output_file_name,
			std::ios::binary
			);
		if (!target_file)
		{
			throw std::runtime_error("Could not open target file " + output_file_name);
		}

		p0::intermediate::save_unit(
			target_file,
			compiled_unit,
			with_jump_labels
			);
	}
	catch (std::runtime_error const &e)
	{
		std::cerr << e.what() << '\n';
		return 1;
	}
}

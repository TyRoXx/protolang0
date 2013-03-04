#include "p0compile/compiler.hpp"
#include "p0compile/compiler_error.hpp"
#include "p0compile/pretty_print_error.hpp"
#include "p0compile/compile_unit.hpp"
#include "p0i/save_unit.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <boost/program_options.hpp>
using namespace std;

int main(int argc, char **argv)
{
	namespace po = boost::program_options;

	std::string source_file_name;
	std::string output_file_name = "out.p0i";

	po::options_description desc("");
	desc.add_options()
		("help", "produce help message")
		("source,s", po::value<std::string>(&source_file_name), "source file name")
		("out,o", po::value<std::string>(&output_file_name), "output file name")
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

	try
	{
		auto const compiled_unit = p0::compile_unit_from_file(source_file_name);

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
			compiled_unit
			);
	}
	catch (std::runtime_error const &e)
	{
		std::cerr << e.what() << '\n';
		return 1;
	}
}

#include "p0i/unit.hpp"
#include "p0compile/compile_unit.hpp"
#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>

namespace
{
	p0::intermediate::unit load_unit(std::string const &file_name)
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

	void execute(p0::intermediate::unit const &unit)
	{
		assert(!"not implemented");
	}
}

int main(int argc, char **argv)
{
	namespace po = boost::program_options;

	std::string file_name;

	po::options_description desc("");
	desc.add_options()
		("help", "produce help message")
		("file,f", po::value<std::string>(&file_name), "file name")
		("compile,c", "")
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
				: load_unit(file_name);
		execute(unit);
	}
	catch (std::exception const &ex)
	{
		std::cerr << ex.what() << "\n";
		return 1;
	}
}

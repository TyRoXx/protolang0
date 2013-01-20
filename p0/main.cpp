#include "p0i/unit.hpp"
#include <iostream>
#include <fstream>


namespace
{
	std::ostream &log()
	{
		return std::cerr;
	}

	void print_help()
	{
	}

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
}

int main(int argc, char **argv)
{
	if (argc <= 1)
	{
		print_help();
		return 0;
	}

	try
	{
		std::string const unit_file_name = argv[1];
		auto const unit = load_unit(unit_file_name);
	}
	catch (std::exception const &ex)
	{
		log() << ex.what() << "\n";
		return 1;
	}
}

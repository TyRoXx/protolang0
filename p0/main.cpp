#include <boost/property_tree/json_parser.hpp>
#include "p0i/ptree_load_unit.hpp"
#include "p0i/unit.hpp"


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

		boost::property_tree::ptree unit_tree;
		boost::property_tree::read_json(
			unit_file,
			unit_tree
			);

		return p0::intermediate::load_unit(unit_tree);
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

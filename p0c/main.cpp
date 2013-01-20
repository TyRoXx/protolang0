#include "p0compile/compiler.hpp"
#include "p0compile/compiler_error.hpp"
#include "p0compile/pretty_print_error.hpp"
#include "p0i/save_unit.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <boost/program_options.hpp>
using namespace std;

namespace
{
	std::vector<char> read_file(std::string const &path)
	{
		std::ifstream file(path, std::ios::binary);
		if (!file)
		{
			throw std::runtime_error("Could not open file " + path);
		}
		return std::vector<char>(
			std::istreambuf_iterator<char>(file),
			std::istreambuf_iterator<char>()
			);
	}
}

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

	auto &error_out = std::cerr;
	try
	{
		auto const source_file_content = read_file(source_file_name);

		p0::source_range const source(
			source_file_content.data(),
			source_file_content.data() + source_file_content.size()
			);

		size_t error_counter = 0;
		auto const handle_error = [&](p0::compiler_error const &error)
		{
			++error_counter;
			pretty_print_error(
				error_out,
				source,
				error
			);
			return true;
		};

		try
		{
			auto const integer_width = 64;

			p0::compiler compiler(
				source,
				integer_width,
				handle_error
				);

			p0::intermediate::unit const compiled_unit = compiler.compile();

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
		catch (p0::compiler_error const &e)
		{
			handle_error(e);
		}

		if (error_counter)
		{
			error_out << error_counter << " error";
			if (error_counter != 1)
			{
				error_out << 's';
			}
			error_out << '\n';
			return 1;
		}
	}
	catch (std::runtime_error const &e)
	{
		error_out << e.what() << '\n';
		return 1;
	}
}

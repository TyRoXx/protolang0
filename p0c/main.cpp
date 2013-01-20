#include "p0compile/compiler.hpp"
#include "p0compile/compiler_error.hpp"
#include "p0compile/pretty_print_error.hpp"
#include "p0i/save_unit.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
using namespace std;

namespace
{
	std::string const default_output_filename = "out.p0i";

	void print_help()
	{
		cout << "p0c [source file] [target file, default " << default_output_filename << "]\n";
	}

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
	if (argc <= 1)
	{
		print_help();
		return 0;
	}

	auto &error_out = std::cerr;
	try
	{
		std::string const source_file_name = argv[1];
		auto const source_file_content = read_file(source_file_name);
		std::string const target_file_name = (argc >= 3) ?
			argv[2] : default_output_filename;

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
				target_file_name,
				std::ios::binary
				);
			if (!target_file)
			{
				throw std::runtime_error("Could not open target file " + target_file_name);
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
				error_out << "s";
			}
			error_out << "\n";
			return 1;
		}
	}
	catch (std::runtime_error const &e)
	{
		error_out << e.what() << '\n';
		return 1;
	}
}

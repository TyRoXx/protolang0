#include "compile_unit.hpp"
#include "compiler.hpp"
#include "compiler_error.hpp"
#include "pretty_print_error.hpp"
#include <fstream>

namespace p0
{
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

	intermediate::unit compile_unit(source_range const &source)
	{
		auto &error_out = std::cerr;
		size_t error_counter = 0;
		auto const handle_error = [&](p0::compiler_error const &error) -> bool
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

			p0::intermediate::unit compiled_unit = compiler.compile();
			return compiled_unit;
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
		}

		throw std::runtime_error(""); //TODO
	}

	intermediate::unit compile_unit_from_file(std::string const &file_name)
	{
		auto const source_file_content = read_file(file_name);

		p0::source_range const source(
			source_file_content.data(),
			source_file_content.data() + source_file_content.size()
			);

		return compile_unit(source);
	}
}

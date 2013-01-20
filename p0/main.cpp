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

	struct interpreter
	{
		explicit interpreter(p0::intermediate::unit const &program);
		void call(p0::intermediate::function const &function);

	private:

		p0::intermediate::unit const &m_program;
	};

	interpreter::interpreter(p0::intermediate::unit const &program)
		: m_program(program)
	{
	}

	void interpreter::call(p0::intermediate::function const &function)
	{
		assert(!"not implemented");
	}

	void execute(p0::intermediate::unit const &program,
				 p0::intermediate::function const &entry_point)
	{
		interpreter interpreter(program);
		interpreter.call(entry_point);
	}
}

int main(int argc, char **argv)
{
	namespace po = boost::program_options;

	std::string file_name;
	std::size_t entry_point_id = 0;

	po::options_description desc("");
	desc.add_options()
		("help", "produce help message")
		("file,f", po::value<std::string>(&file_name), "file name")
		("compile,c", "")
		("entry,e", po::value<std::size_t>(&entry_point_id), "entry function id")
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

		p0::intermediate::function const *entry_point = 0;
		if (entry_point_id < unit.functions().size())
		{
			entry_point = &unit.functions()[entry_point_id];
		}
		else
		{
			std::cerr << "Entry function " << entry_point_id
					  << " does not exist\n";
			return 1;
		}

		execute(unit, *entry_point);
	}
	catch (std::exception const &ex)
	{
		std::cerr << ex.what() << "\n";
		return 1;
	}
}

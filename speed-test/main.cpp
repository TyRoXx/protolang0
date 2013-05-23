#include "p0compile/compile_unit.hpp"
#include "p0run/interpreter.hpp"
#include "p0run/default_garbage_collector.hpp"
#include "p0run/runtime_error.hpp"
#include "p0run/runtime_error_exception.hpp"
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/date_time/microsec_time_clock.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/date_duration_operators.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <boost/format.hpp>


namespace p0
{
	template <class F>
	boost::posix_time::time_duration measure_running_duration(F const &action)
	{
		auto const clock = boost::date_time::microsec_clock<boost::posix_time::ptime>();
		auto const start = clock.universal_time();
		action();
		auto const finish = clock.universal_time();
		return (finish - start);
	}

	void print_running_duration(std::ostream &out,
								boost::posix_time::time_duration duration)
	{
		out << duration;
	}

	template <class ValuePredicate>
	bool run_test(std::string const &test_name,
				  std::string const &source,
				  ValuePredicate const &check_result,
				  std::ostream &out)
	{
		source_range const source_range((source.data()), source.data() + source.size());
		auto const program = compile_unit(source_range);
		run::default_garbage_collector gc;
		run::interpreter interpreter(gc, nullptr);
		run::value result;
		auto const duration = measure_running_duration([&result, &program, &interpreter]()
		{
			result = interpreter.call(
				intermediate::function_ref(program, program.functions().front()), {});
		});
		out << test_name << " ";
		bool const is_success = check_result(result);
		out << (is_success ? "succeeded" : "failed") << " in ";
		print_running_duration(out, duration);
		out << '\n';
		return is_success;
	}

	void run_sum_test(run::integer max, std::ostream &out)
	{
		run_test((boost::format("sum %1%") % max).str(),
				 (boost::format(
					"var sum = 0\n"
					"var i = 1\n"
					"while i <= %1% {\n"
					"    sum = sum + i\n"
					"    i = i + 1\n"
					"}\n"
					"return sum\n"
				 ) % max).str(),
				 [max](run::value result)
		{
			run::integer const expected = ((max + 1) * (max / 2));
			return result == run::value(expected);
		}, out);
	}

	void run_all_tests(std::ostream &out)
	{
		run_test("trivial", "return 5", [](run::value result)
		{
			return result == run::value(5);
		}, out);

		run_sum_test(100000, out);
		run_sum_test(1000000, out);
		run_sum_test(10000000, out);
	}
}

int main(int argc, char **argv)
{
	namespace po = boost::program_options;

	auto &err = std::cerr;

	po::options_description desc("");
	desc.add_options()
		("help", "produce help message")
		;

	po::variables_map vm;
	try
	{
		po::store(
			po::command_line_parser(argc, argv).options(desc).run(),
			vm);
		po::notify(vm);
	}
	catch (po::error const &e)
	{
		err << e.what() << "\n";
		return 1;
	}

	if (vm.count("help"))
	{
		err << desc << "\n";
		return 0;
	}

	try
	{
		p0::run_all_tests(std::cerr);
	}
	catch (std::exception const &ex)
	{
		err << ex.what() << "\n";
		return 1;
	}
}

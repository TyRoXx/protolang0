#include "p0compile/parser.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/filesystem/operations.hpp>
#include <iostream>


namespace
{
	void run_test_scripts()
	{
	}
}

BOOST_AUTO_TEST_CASE(test_scripts)
{
	if (boost::filesystem::exists("./test-scripts"))
	{
		run_test_scripts();
	}
	else
	{
		std::cerr
			<< "Put the 'test-scripts' directory into working directory "
			   "(e.g. with a soft link) to enable execution of the contained "
			   "scripts.\n";
	}
}

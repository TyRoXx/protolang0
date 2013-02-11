#include "p0compile/parser.hpp"
#include "p0compile/expression_tree.hpp"
#include "p0compile/scanner.hpp"
#include "p0compile/compiler_error.hpp"
#include <boost/test/unit_test.hpp>


namespace
{
	bool handle_unexpected_error(p0::compiler_error const &error)
	{
		(void)error;
		BOOST_CHECK(nullptr == "No error expected");
		return true;
	}
}

BOOST_AUTO_TEST_CASE(parse_empty_file_test)
{
	std::string const source = "";
	p0::source_range const source_range(
				source.data(),
				source.data() + source.size());
	p0::scanner scanner(source_range);
	p0::parser parser(scanner, handle_unexpected_error);
	auto const ast = parser.parse_unit();
	BOOST_REQUIRE(ast);
}

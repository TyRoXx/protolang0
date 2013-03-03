#include "p0compile/scanner.hpp"
#include "p0compile/compiler_error.hpp"
#include <boost/test/unit_test.hpp>


namespace
{
	bool check_expected_exception(
		std::string const &code,
		std::size_t position
		)
	{
		p0::scanner scanner(p0::source_range(code.data(), code.data() + code.size()));

		try
		{
			while (scanner.next_token().type != p0::token_type::end_of_file)
			{
			}
		}
		catch (p0::compiler_error const &e)
		{
			auto const error_pos = static_cast<size_t>(
				std::distance(code.data(), e.position().begin()));
			return (position == error_pos);
		}

		return false;
	}
}

BOOST_AUTO_TEST_CASE(general_error_test)
{
	BOOST_CHECK(check_expected_exception("abc?", 3));
	BOOST_CHECK(check_expected_exception("?", 0));
	BOOST_CHECK(check_expected_exception("\\", 0));
	BOOST_CHECK(check_expected_exception("123e", 3));
}

BOOST_AUTO_TEST_CASE(string_error_test)
{
	//these characters may not occur unescaped in a string
	BOOST_CHECK(check_expected_exception("\"\xff\"", 1));
	BOOST_CHECK(check_expected_exception("\"\x7f\"", 1));
	BOOST_CHECK(check_expected_exception("\"\x1f\"", 1));
	BOOST_CHECK(check_expected_exception("\"\x00\"", 1));
	BOOST_CHECK(check_expected_exception("\"\x05\"", 1));
	BOOST_CHECK(check_expected_exception("\"\x0a\"", 1));

	//missing closing quote
	BOOST_CHECK(check_expected_exception("\"hallo", 6));

	//missing escape character(s) after backslash
	BOOST_CHECK(check_expected_exception("\"hallo\\", 7));

	//the second quote is escaped so it is expected not to close the string
	BOOST_CHECK(check_expected_exception("\"hallo\\\"", 8));
}

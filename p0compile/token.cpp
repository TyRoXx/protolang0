#include "token.hpp"


namespace p0
{
	token::token() PROTOLANG0_NOEXCEPT
		: type(token_type::end_of_file)
	{
	}

	token::token(token_type::Enum type, source_range content) PROTOLANG0_NOEXCEPT
		: type(type)
		, content(content)
	{
	}
}

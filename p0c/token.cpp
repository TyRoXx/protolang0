#include "token.hpp"


namespace p0
{
	token::token()
		: type(token_type::end_of_file)
	{
	}

	token::token(token_type::Enum type, source_range content)
		: type(type)
		, content(content)
	{
	}
}

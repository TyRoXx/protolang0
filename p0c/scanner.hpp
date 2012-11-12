#pragma once
#ifndef P0C_SCANNER_HPP
#define P0C_SCANNER_HPP


#include "p0i/unit.hpp"
#include "source_range.hpp"
#include "token.hpp"


namespace p0
{
	struct scanner
	{
		explicit scanner(
			source_range source
			);
		token next_token();
		void skip_line();
		source_range rest() const;

	private:

		source_range::iterator m_pos, m_end;


		void skip_whitespace();
		token eat_single_char_token(token_type::Enum type);
		token eat_single_or_double_token(
			token_type::Enum single_token,
			char second_char,
			token_type::Enum double_token
			);
		token eat_single_or_triple_token(
			token_type::Enum single_token,
			char second_char_0,
			token_type::Enum double_token_0,
			char second_char_1,
			token_type::Enum double_token_1
			);
	};
}


#endif

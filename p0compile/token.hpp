#pragma once
#ifndef P0C_TOKEN_HPP
#define P0C_TOKEN_HPP


#include "source_range.hpp"
#include "p0common/final.hpp"


namespace p0
{
	namespace token_type
	{
		enum Enum
		{
			identifier,
			var,
			function,
			return_,
			null,
			if_,
			else_,
			while_,
			break_,
			continue_,
			import,
			integer_10,
			string,
			parenthesis_left,
			parenthesis_right,
			brace_left,
			brace_right,
			bracket_left,
			bracket_right,
			assign,
			comma,
			plus,
			minus,
			star,
			slash,
			modulo,
			ampersand,
			ampersands,
			pipe,
			pipes,
			shift_left,
			shift_right,
			shift_signed,
			less,
			less_equal,
			greater,
			greater_equal,
			equal,
			not_equal,
			exclamation_mark,
			dot,
			tilde,
			caret,
			end_of_file,
		};
	}


	struct token PROTOLANG0_FINAL_CLASS
	{
		token_type::Enum type;
		source_range content;


		token();
		explicit token(token_type::Enum type, source_range content);
	};
}


#endif

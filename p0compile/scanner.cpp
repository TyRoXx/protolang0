#include "scanner.hpp"
#include "compiler_error.hpp"
#include <cctype>
#include <cstring>


namespace p0
{
	namespace
	{
		bool is_whitespace(char c)
		{
			switch (c)
			{
			case ' ':
			case '\t':
			case '\n':
			case '\r':
				return true;

			default:
				return false;
			}
		}

		bool is_identifer_first(char c)
		{
			c = static_cast<char>(std::tolower(c));
			return
				((c >= 'a') && (c <= 'z')) ||
				(c == '_');
		}

		bool is_digit_10(char c)
		{
			return ((c >= '0') && (c <= '9'));
		}

		bool is_identifier_tail(char c)
		{
			return
				is_identifer_first(c) ||
				is_digit_10(c);
		}

		bool is_keyword(source_range content, char const *keyword)
		{
			for (auto i = content.begin(); i != content.end(); ++i, ++keyword)
			{
				if (*i != *keyword)
				{
					return false;
				}
			}

			return (*keyword == '\0');
		}

		struct keyword
		{
			char const *content;
			token_type::Enum token;
		};

		token_type::Enum find_keyword(source_range content)
		{
			//a rather simple (= slow) approach of finding keywords
			static std::array<keyword, 10> const keywords =
			{{
				{"var", token_type::var},
				{"function", token_type::function},
				{"return", token_type::return_},
				{"null", token_type::null},
				{"if", token_type::if_},
				{"else", token_type::else_},
				{"while", token_type::while_},
				{"break", token_type::break_},
				{"continue", token_type::continue_},
				{"import", token_type::import},
			}};

			using namespace std;

			for (auto k = begin(keywords); k != end(keywords); ++k)
			{
				if (is_keyword(content, k->content))
				{
					return k->token;
				}
			}

			return token_type::identifier;
		}
	}


	scanner::scanner(
		source_range source
		)
		: m_pos(source.begin())
		, m_end(source.end())
		, m_was_integer(false)
	{
	}

	token scanner::next_token()
	{
		skip_whitespace();

		if (m_pos == m_end)
		{
			return token(
				token_type::end_of_file,
				source_range(m_end, m_end)
				);
		}

		bool const was_integer = m_was_integer;
		m_was_integer = false;

		switch (*m_pos)
		{
		case '(': return eat_single_char_token(token_type::parenthesis_left);
		case ')': return eat_single_char_token(token_type::parenthesis_right);
		case '{': return eat_single_char_token(token_type::brace_left);
		case '}': return eat_single_char_token(token_type::brace_right);
		case '[': return eat_single_char_token(token_type::bracket_left);
		case ']': return eat_single_char_token(token_type::bracket_right);
		case '=': return eat_single_or_double_token(token_type::assign, '=', token_type::equal);
		case ',': return eat_single_char_token(token_type::comma);
		case '+': return eat_single_char_token(token_type::plus);
		case '-': return eat_single_char_token(token_type::minus);
		case '*': return eat_single_char_token(token_type::star);
		case '/': return eat_single_char_token(token_type::slash);
		case '%': return eat_single_char_token(token_type::modulo);
		case '&': return eat_single_or_double_token(
						token_type::ampersand,
						'&', token_type::ampersands);
		case '|': return eat_single_or_double_token(
						token_type::pipe,
						'|', token_type::pipes);
		case '<': return eat_single_or_double_token(
						token_type::less,
						'<', token_type::shift_left,
						'=', token_type::less_equal);
		case '>': return eat_triple_token(
						token_type::greater,
						'>', token_type::shift_right,
						'=', token_type::greater_equal,
						">>", token_type::shift_signed);
		case '!': return eat_single_or_double_token(
						token_type::exclamation_mark,
						'=', token_type::not_equal);
		case '.': return eat_single_char_token(token_type::dot);
		case '~': return eat_single_char_token(token_type::tilde);
		case '^': return eat_single_char_token(token_type::caret);
		case '"':
			{
				++m_pos;
				auto const string_begin = m_pos;

				for (;;)
				{
					auto const on_unexpected_eof = [this]()
					{
						throw compiler_error(
							"Unexpected end of file in string literal",
							source_range(m_end, m_end)
							);
					};

					if (m_pos == m_end)
					{
						on_unexpected_eof();
					}

					if (((*m_pos < 0x20) ||
						(*m_pos > 0x7e)) &&
						(*m_pos != '\t'))
					{
						throw compiler_error(
							"This character has to be hex-encoded in a string literal",
							source_range(m_pos, m_pos + 1)
							);
					}

					if (*m_pos == '\\')
					{
						++m_pos;
						if (m_pos == m_end)
						{
							on_unexpected_eof();
						}

						++m_pos;
					}

					else if (*m_pos == '"')
					{
						break;
					}

					else
					{
						++m_pos;
					}
				}

				auto const string_end = m_pos;
				++m_pos;

				return token(
					token_type::string,
					source_range(string_begin, string_end)
					);
			}

		default:
			if (is_identifer_first(*m_pos))
			{
				if (was_integer)
				{
					throw compiler_error(
						"An integer may not be directly followed by an identifier or a keyword",
						source_range(m_pos, m_pos + 1)
						);
				}

				auto const identifier_begin = m_pos;
				do
				{
					++m_pos;
				}
				while (
					(m_pos != m_end) &&
					is_identifier_tail(*m_pos));
				auto const identifier_end = m_pos;

				source_range const range(
					identifier_begin,
					identifier_end
					);

				auto const type = find_keyword(range);

				return token(
					type,
					range
					);
			}

			if (is_digit_10(*m_pos))
			{
				auto const integer_begin = m_pos;
				do
				{
					++m_pos;
				}
				while (
					(m_pos != m_end) &&
					is_digit_10(*m_pos));
				auto const integer_end = m_pos;

				m_was_integer = true;

				return token(
					token_type::integer_10,
					source_range(integer_begin, integer_end)
					);
			}

			throw compiler_error(
				"Unexpected character",
				source_range(m_pos, m_pos + 1)
				);
		}
	}

	void scanner::skip_line()
	{
		while (
			(m_pos != m_end) &&
			(*m_pos != '\n'))
		{
			++m_pos;
		}
	}

	source_range scanner::rest() const
	{
		return source_range(
			m_pos,
			m_end
			);
	}


	void scanner::skip_whitespace()
	{
		while (m_pos != m_end)
		{
			if (is_whitespace(*m_pos))
			{
				++m_pos;

				m_was_integer = false;
				continue;
			}

			//a comment starts with a semicolon and ends after the current line
			if (*m_pos == ';')
			{
				do
				{
					++m_pos;
				}
				while (
					(m_pos != m_end) &&
					(*m_pos != '\n')
					);

				m_was_integer = false;
				continue;
			}

			break;
		}
	}

	token scanner::eat_single_char_token(token_type::Enum type)
	{
		++m_pos;
		return token(
			type,
			source_range((m_pos - 1), m_pos)
			);
	}

	token scanner::eat_single_or_double_token(
		token_type::Enum single_token,
		char second_char,
		token_type::Enum double_token
		)
	{
		++m_pos;

		if ((m_pos != m_end) &&
			(*m_pos == second_char))
		{
			++m_pos;
			return token(double_token, source_range(m_pos - 2, m_pos));
		}

		return token(single_token, source_range(m_pos - 1, m_pos));
	}

	token scanner::eat_single_or_double_token(
		token_type::Enum single_token,
		char second_char_0,
		token_type::Enum double_token_0,
		char second_char_1,
		token_type::Enum double_token_1
		)
	{
		return eat_triple_token(single_token,
								second_char_0, double_token_0,
								second_char_1, double_token_1,
								nullptr, token_type::end_of_file);
	}

	token scanner::eat_triple_token(
		token_type::Enum single_token,
		char second_char_0,
		token_type::Enum double_token_0,
		char second_char_1,
		token_type::Enum double_token_1,
		char const *tail,
		token_type::Enum tail_token
		)
	{
		++m_pos;

		if (m_pos != m_end)
		{
			if (tail)
			{
				auto const tail_size = static_cast<ptrdiff_t>(std::strlen(tail));
				if (tail_size <= std::distance(m_pos, m_end))
				{
					auto const tail_end = tail + tail_size;
					auto const result = std::mismatch(
						tail, tail_end,
						m_pos);

					//do we have a match?
					if (result.first == tail_end)
					{
						auto const token_begin = m_pos;
						m_pos = result.second;
						return token(tail_token, source_range(token_begin, m_pos));
					}
				}
			}

			if (*m_pos == second_char_0)
			{
				++m_pos;
				return token(double_token_0, source_range(m_pos - 2, m_pos));
			}
			if (*m_pos == second_char_1)
			{
				++m_pos;
				return token(double_token_1, source_range(m_pos - 2, m_pos));
			}
		}

		return token(single_token, source_range(m_pos - 1, m_pos));
	}
}

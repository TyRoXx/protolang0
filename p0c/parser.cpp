#include "parser.hpp"
#include "compiler_error.hpp"
#include "scanner.hpp"
#include "expression_tree.hpp"
#include "statement_tree.hpp"
#include <cassert>


namespace p0
{
	parser::parser(
		scanner &scanner,
		compiler_error_handler error_handler
		)
		: m_scanner(scanner)
		, m_error_handler(std::move(error_handler))
		, m_is_next_token(false)
	{
		assert(m_error_handler);
	}

	std::unique_ptr<function_tree> parser::parse_unit()
	{
		auto main_function = parse_function(
			m_scanner.rest().begin()
			);
		skip_token(
			token_type::end_of_file,
			"End of file after main function expected"
			);
		return main_function;
	}


	std::unique_ptr<statement_tree> parser::parse_statement()
	{
		token const first = peek_token();
		switch (first.type)
		{
		case token_type::var:
			{
				pop_token();
				token const name_token = pop_token();
				expect_token_type(
					name_token,
					token_type::identifier,
					"Name of declared variable expected"
					);

				skip_token(
					token_type::assign,
					"Assignment operator '=' expected"
					);

				auto value = parse_expression();
				return std::unique_ptr<statement_tree>(new declaration_tree(
					name_token.content,
					std::move(value)
					));
			}

		case token_type::return_:
			{
				pop_token();
				auto value = parse_expression();
				return std::unique_ptr<statement_tree>(new return_tree(
					std::move(value)
					));
			}

		case token_type::brace_left:
			{
				pop_token();
				auto body = parse_block();
				return std::move(body);
			}

		case token_type::if_:
			{
				pop_token();
				auto condition = parse_expression();

				skip_token(
					token_type::brace_left,
					"The 'if' block cannot be a single statement. '{' expected"
					);
				auto on_true = parse_block();

				std::unique_ptr<statement_tree> on_false;
				if (try_skip_token(
					token_type::else_
					))
				{
					skip_token(
						token_type::brace_left,
						"The 'else' block cannot be a single statement. '{' expected"
						);
					on_false = parse_block();
				}

				return std::unique_ptr<statement_tree>(new if_tree(
					std::move(condition),
					std::move(on_true),
					std::move(on_false)
					));
			}

		case token_type::while_:
			{
				pop_token();
				auto condition = parse_expression();

				skip_token(
					token_type::brace_left,
					"The 'while' block cannot be a single statement. '{' expected"
					);
				auto body = parse_block();

				return std::unique_ptr<statement_tree>(new while_tree(
					std::move(condition),
					std::move(body)
					));
			}

		case token_type::break_:
			{
				pop_token();
				return std::unique_ptr<statement_tree>(new break_tree(
					));
			}

		case token_type::continue_:
			{
				pop_token();
				return std::unique_ptr<statement_tree>(new continue_tree(
					));
			}

		default:
			{
				auto left = parse_expression();

				if (try_skip_token(token_type::assign))
				{
					auto right = parse_expression();

					return std::unique_ptr<statement_tree>(new assignment_tree(
						std::move(left),
						std::move(right)
						));
				}
				else
				{
					return std::unique_ptr<statement_tree>(new expression_statement_tree(
						std::move(left)
						));
				}
			}
		}
	}

	struct end_of_file_error : compiler_error
	{
		explicit end_of_file_error(
			std::string message,
			source_range position
			)
			: compiler_error(std::move(message), position)
		{
		}
	};

	std::unique_ptr<statement_tree> parser::parse_block()
	{
		block_tree::statement_vector body;

		//this block is only left with:
		//	break - on the closing '}'
		//	end_of_file_error - becase the block is incomplete in the input
		//	compiler_error - when the error handling function returns false
		for (;;)
		{
			try
			{
				if (peek_token().type == token_type::end_of_file)
				{
					throw end_of_file_error(
						"Unexpected end of file in block",
						m_scanner.next_token().content
						);
				}

				//end of the block
				if (try_skip_token(
					token_type::brace_right
					))
				{
					break;
				}

				auto statement = parse_statement();
				body.push_back(
					std::move(statement)
					);
			}
			catch (end_of_file_error const &)
			{
				//this error is best handled at the top-most level
				throw;
			}
			catch (compiler_error const &e)
			{
				if (!m_error_handler(e))
				{
					throw;
				}

				//Try again at the next line to find more potential errors.
				//The next statement does not necessarily start on the very next
				//line, but this is a good approximation for real world sources.
				skip_line();
			}
		}

		return std::unique_ptr<statement_tree>(new block_tree(
			std::move(body)
			));
	}

	namespace
	{
		infix_precedence const strongest_precedence = 1;
		infix_precedence const weakest_precedence = 14;


		infix_precedence get_stronger(infix_precedence precedence)
		{
			assert(precedence > strongest_precedence);
			return (precedence - 1);
		}

		bool is_acceptable_precedence(infix_precedence found, infix_precedence weakest)
		{
			return (found <= weakest);
		}

		bool is_infix_operator(
			token_type::Enum token,
			infix_precedence &precedence,
			binary_operator::Enum &operator_)
		{
			switch (token)
			{
			case token_type::pipes:
				precedence = 14;
				operator_ = binary_operator::logical_or;
				return true;

			case token_type::ampersands:
				precedence = 13;
				operator_ = binary_operator::logical_and;
				return true;

			case token_type::pipe:
				precedence = 12;
				operator_ = binary_operator::bit_or;
				return true;

			case token_type::caret:
				precedence = 11;
				operator_ = binary_operator::bit_xor;
				return true;

			case token_type::ampersand:
				precedence = 10;
				operator_ = binary_operator::bit_and;
				return true;

			case token_type::equal:
				precedence = 9;
				operator_ = binary_operator::equal;
				return true;

			case token_type::not_equal:
				precedence = 9;
				operator_ = binary_operator::not_equal;
				return true;

			case token_type::less:
				precedence = 8;
				operator_ = binary_operator::less;
				return true;

			case token_type::less_equal:
				precedence = 8;
				operator_ = binary_operator::less_equal;
				return true;

			case token_type::greater:
				precedence = 8;
				operator_ = binary_operator::greater;
				return true;

			case token_type::greater_equal:
				precedence = 8;
				operator_ = binary_operator::greater_equal;
				return true;

			case token_type::shift_left:
				precedence = 7;
				operator_ = binary_operator::shift_left;
				return true;

			case token_type::shift_right:
				precedence = 7;
				operator_ = binary_operator::shift_right;
				return true;

			case token_type::plus:
				precedence = 6;
				operator_ = binary_operator::add;
				return true;

			case token_type::minus:
				precedence = 6;
				operator_ = binary_operator::sub;
				return true;

			case token_type::star:
				precedence = 5;
				operator_ = binary_operator::mul;
				return true;

			case token_type::slash:
				precedence = 5;
				operator_ = binary_operator::div;
				return true;

			case token_type::modulo:
				precedence = 5;
				operator_ = binary_operator::mod;
				return true;

			default:
				return false;
			}
		}
	}

	std::unique_ptr<expression_tree> parser::parse_expression()
	{
		return parse_infix_expression(weakest_precedence);
	}

	std::unique_ptr<expression_tree> parser::parse_infix_expression(infix_precedence weakest_acceptable_precedence)
	{
		auto left = parse_extended_primary_expression();

		for (;;)
		{
			auto const potential_operator = peek_token();

			infix_precedence precedence;
			binary_operator::Enum operator_;

			if (is_infix_operator(
				potential_operator.type,
				precedence,
				operator_) &&
				is_acceptable_precedence(precedence, weakest_acceptable_precedence))
			{
				pop_token();
				auto right = parse_infix_expression(get_stronger(precedence));

				auto const position = left->position(); //TODO

				std::unique_ptr<expression_tree> infix_expr(new binary_expression_tree(
					operator_,
					std::move(left),
					std::move(right),
					position));

				left = std::move(infix_expr);
			}
			else
			{
				break;
			}
		}

		return left;
	}

	std::unique_ptr<expression_tree> parser::parse_extended_primary_expression()
	{
		auto left = parse_primary_expression();

		for (;;)
		{
			if (try_skip_token(token_type::parenthesis_left))
			{
				//postfix call syntax like in C++
				//f(a, b, c)
				//left = f
				//arguments = {a, b, c}

				call_expression_tree::expression_vector arguments;

				for (;;)
				{
					if (try_skip_token(token_type::parenthesis_right))
					{
						break;
					}

					auto argument = parse_expression();
					arguments.push_back(
						std::move(argument)
						);

					if (!try_skip_token(token_type::comma))
					{
						skip_token(
							token_type::parenthesis_right,
							"Comma or closing parenthesis expected"
							);
						break;
					}
				}

				std::unique_ptr<expression_tree> call(new call_expression_tree(
					std::move(left),
					std::move(arguments)
					));

				left = std::move(call);
			}

			else if (try_skip_token(token_type::dot))
			{
				auto const element_name = pop_token();
				expect_token_type(
					element_name,
					token_type::identifier,
					"Element name expected"
					);

				std::unique_ptr<expression_tree> access(new dot_element_expression_tree(
					std::move(left),
					element_name.content
					));

				left = std::move(access);
			}

			else
			{
				break;
			}
		}

		return left;
	}

	std::unique_ptr<expression_tree> parser::parse_primary_expression()
	{
		auto const first = pop_token();
		switch (first.type)
		{
		case token_type::identifier:
			return std::unique_ptr<expression_tree>(
				new name_expression_tree(
					first.content
				));

		case token_type::integer_10:
			return std::unique_ptr<expression_tree>(
				new integer_10_expression_tree(
					first.content
				));

		case token_type::parenthesis_left:
			{
				//any expression can be enclosed in parentheses
				auto value = parse_expression();
				auto const closing_parenthesis = pop_token();
				expect_token_type(
					closing_parenthesis,
					token_type::parenthesis_right,
					"Closing parenthesis ')' expected"
					);
				return std::move(value);
			}

		case token_type::function:
			{
				return parse_function(
					first.content.begin()
					);
			}

		case token_type::null:
			{
				return std::unique_ptr<expression_tree>(
					new null_expression_tree(first.content)
					);
			}

		case token_type::bracket_left:
			{
				table_expression_tree::element_vector elements;

				//TODO: include full expression
				auto const position = first.content;

				for (;;)
				{
					if (try_skip_token(
						token_type::bracket_right
						))
					{
						break;
					}

					auto const key = pop_token();
					expect_token_type(
						key,
						token_type::identifier,
						"Key identifier expected"
						);

					skip_token(
						token_type::assign,
						"Assignment operator '=' expected"
						);

					auto value = parse_expression();

					elements.push_back(std::make_pair(
						key.content,
						std::move(value)
						));

					if (!try_skip_token(
						token_type::comma
						))
					{
						skip_token(
							token_type::bracket_right,
							"A table literal must end with a closing bracket ']'"
							);
						break;
					}
				}

				return std::unique_ptr<expression_tree>(new table_expression_tree(
					std::move(elements),
					position
					));
			}

		case token_type::minus:
			{
				auto input = parse_extended_primary_expression();
				return std::unique_ptr<expression_tree>(new unary_expression_tree(
					unary_operator::negative,
					std::move(input),
					first.content
					));
			}

		case token_type::plus:
			{
				auto value = parse_extended_primary_expression();
				return value;
			}

		case token_type::exclamation_mark:
			{
				auto input = parse_extended_primary_expression();
				return std::unique_ptr<expression_tree>(new unary_expression_tree(
					unary_operator::not_,
					std::move(input),
					first.content
					));
			}

		case token_type::tilde:
			{
				auto input = parse_extended_primary_expression();
				return std::unique_ptr<expression_tree>(new unary_expression_tree(
					unary_operator::inverse,
					std::move(input),
					first.content
					));
			}

		case token_type::string:
			{
				string_expression_tree::part_vector parts;
				parts.push_back(first.content);

				for (;;)
				{
					auto const next_part = peek_token();
					if (next_part.type != token_type::string)
					{
						break;
					}

					pop_token();
					parts.push_back(next_part.content);
				}

				return std::unique_ptr<expression_tree>(new string_expression_tree(
					std::move(parts)
					));
			}

		default:
			throw compiler_error(
				"Expression expected",
				first.content
				);
		}
	}

	std::unique_ptr<function_tree> parser::parse_function(
		source_range::iterator function_begin
		)
	{
		function_tree::name_vector parameters;

		//the parameter list is optional
		if (try_skip_token(
			token_type::parenthesis_left
			))
		{
			for (;;)
			{
				auto const name = peek_token();
				if (name.type == token_type::identifier)
				{
					pop_token();
					parameters.push_back(name.content);

					if (!try_skip_token(
						token_type::comma
						))
					{
						break;
					}
				}
				else
				{
					break;
				}
			}

			skip_token(
				token_type::parenthesis_right,
				"Closing parenthesis ')' after parameter list expected"
				);
		}

		skip_token(
			token_type::brace_left,
			"Opening brace '{' of function body expected"
			);

		auto body = parse_block();
		return std::unique_ptr<function_tree>(new function_tree(
			std::move(body),
			std::move(parameters),
			source_range(function_begin, function_begin) //TODO: end should point after '}'
			));
	}

	void parser::expect_token_type(token const &token, token_type::Enum type, std::string const &message) const
	{
		if (token.type != type)
		{
			throw compiler_error(
				message,
				token.content
				);
		}
	}

	token const &parser::peek_token()
	{
		if (!m_is_next_token)
		{
			m_next_token = m_scanner.next_token();
			m_is_next_token = true;
		}

		return m_next_token;
	}

	token parser::pop_token()
	{
		if (m_is_next_token)
		{
			m_is_next_token = false;
			return m_next_token;
		}
		else
		{
			return m_scanner.next_token();
		}
	}

	bool parser::try_skip_token(token_type::Enum type)
	{
		auto const token = peek_token();
		if (token.type == type)
		{
			pop_token();
			return true;
		}

		return false;
	}

	void parser::skip_token(token_type::Enum type, char const *message)
	{
		if (!try_skip_token(type))
		{
			throw compiler_error(
				message,
				peek_token().content
				);
		}
	}

	void parser::skip_line()
	{
		m_scanner.skip_line();
		m_is_next_token = false;
	}
}

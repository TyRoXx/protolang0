#include "rvalue_generator.hpp"
#include "local_frame.hpp"
#include "compiler_error.hpp"
#include "function_generator.hpp"
#include "unit_generator.hpp"
#include "temporary.hpp"
#include <sstream>
#include <cctype>


namespace p0
{
	rvalue_generator::rvalue_generator(
		function_generator &function_generator,
		intermediate::emitter &emitter,
		local_frame &frame,
		reference destination
		)
		: m_function_generator(function_generator)
		, m_emitter(emitter)
		, m_frame(frame)
		, m_destination(destination)
	{
	}


	void rvalue_generator::with_arguments(
			std::vector<std::unique_ptr<expression_tree>> const &arguments,
			std::function<void ()> const &handle_result
			)
	{
		auto const argument_count = arguments.size();
		temporary const argument_variables(
			m_frame,
			argument_count
			);
		size_t current_argument_address = argument_variables.address().local_address();

		for (auto arg = arguments.begin(), end = arguments.end();
			arg != end; ++arg, ++current_argument_address)
		{
			try
			{
				rvalue_generator argument(
					m_function_generator,
					m_emitter,
					m_frame,
					reference(current_argument_address)
					);
				(*arg)->accept(argument);
			}
			catch (compiler_error const &e)
			{
				m_function_generator.handle_error(e);
			}
		}

		handle_result();
	}

	void rvalue_generator::visit(name_expression_tree const &expression)
	{
		auto const name = expression.name();

		auto const address = m_frame.emit_read_only(
			source_range_to_string(name),
			name,
			m_destination,
			m_emitter
			);

		using namespace std::rel_ops;

		if (m_destination.is_valid() &&
			(m_destination != address))
		{
			m_emitter.copy(
				m_destination.local_address(),
				address.local_address()
				);
		}
	}

	void rvalue_generator::visit(integer_10_expression_tree const &expression)
	{
		enum
		{
			base = 10,
		};

		typedef intermediate::instruction_argument integer;

		integer value = 0, previous_value = 0;
		integer const integer_mask = std::numeric_limits<integer>::max();

		auto const value_string = expression.value();
		for (auto i = value_string.begin(); i != value_string.end(); ++i)
		{
			value *= base;
			value += (*i - '0');
			value &= integer_mask;

			if (value < previous_value)
			{
				std::ostringstream message;
				message
					<< "The integer literal is too large (maximum: 2^"
					<< 64
					<< "-1 = "
					<< integer_mask
					<< ")";

				throw compiler_error(
					message.str(),
					expression.position()
					);
			}

			previous_value = value;
		}

		if (m_destination.is_valid())
		{
			m_emitter.set_constant(
				m_destination.local_address(),
				value
			);
		}
	}

	namespace
	{
		unsigned hex_digit_value(source_range::iterator position)
		{
			auto const c = std::tolower(*position);

			if (c >= '0' && c <= '9')
			{
				return static_cast<unsigned>(c - '0');
			}

			if (c >= 'a' && c <= 'f')
			{
				return static_cast<unsigned>(10 + (c - 'a'));
			}

			throw compiler_error(
				"Hexadecimal digit expected",
				source_range(position, position + 1)
				);
		}

		void decode_string_literal(
			std::string &decoded,
			source_range literal
			)
		{
			for (auto i = literal.begin(), end = literal.end(); i != end; ++i)
			{
				auto c = *i;

				assert(c != '"'); //scanner guarantees this

				switch (c)
				{
				case '\\':
					{
						++i;
						assert(i != literal.end());

						auto const second = *i;

						switch (second)
						{
						case 't': c = '\t'; break;
						case 'n': c = '\n'; break;
						case 'r': c = '\r'; break;
						case '0': c = '\0'; break;

						case '\\':
						case '\'':
						case '\"':
							c = second;
							break;

						case 'x':
							{
								++i;
								if (i != end)
								{
									auto hex_value = hex_digit_value(i) * 16u;

									++i;
									if (i != end)
									{
										hex_value += hex_digit_value(i);
										c = static_cast<char>(hex_value);
										break;
									}
								}

								throw compiler_error(
									"Hexidecimal digit expected in '\\xhh' escape sequence",
									source_range(i, i)
									);
							}

						default:
							throw compiler_error(
								"Invalid escape sequence",
								source_range(i - 1, i + 1)
								);
						}
						break;
					}

				default:
					break;
				}

				decoded += c;
			}
		}
	}

	void rvalue_generator::visit(string_expression_tree const &expression)
	{
		std::string value;

		for (auto p = expression.parts().begin(), end = expression.parts().end();
			p != end; ++p)
		{
			decode_string_literal(
				value,
				*p
				);
		}

		if (m_destination.is_valid())
		{
			auto const string_id = m_function_generator.unit().get_string_id(
				std::move(value)
				);

			m_emitter.set_string(
				m_destination.local_address(),
				string_id
				);
		}
	}

	void rvalue_generator::visit(call_expression_tree const &expression)
	{
		temporary const function_variable(
			m_frame,
			1
			);
		auto &result_variable = function_variable;

		try
		{
			rvalue_generator function(
				m_function_generator,
				m_emitter,
				m_frame,
				function_variable.address()
				);
			expression.function().accept(function);
		}
		catch (compiler_error const &e)
		{
			m_function_generator.handle_error(e);
		}

		with_arguments(expression.arguments(),
					   [&]()
		{
			m_emitter.call(
				result_variable.address().local_address(),
				expression.arguments().size()
				);

			if (m_destination.is_valid())
			{
				m_emitter.copy(
					m_destination.local_address(),
					result_variable.address().local_address()
					);
			}
		});
	}

	void rvalue_generator::visit(function_tree const &expression)
	{
		function_generator function_generator(
			m_function_generator, //parent
			&m_frame //outer_frame
			);

		auto const function_index =
				function_generator.generate_function(expression);

		if (m_destination.is_valid())
		{
			m_emitter.set_function(
				m_destination.local_address(),
				function_index
				);

			function_generator.emit_bindings(
				m_destination.local_address(),
				m_frame,
				m_emitter
				);
		}
	}

	void rvalue_generator::visit(null_expression_tree const & /*expression*/)
	{
		if (m_destination.is_valid())
		{
			m_emitter.set_null(
				m_destination.local_address()
				);
		}
	}

	void rvalue_generator::visit(table_expression_tree const &expression)
	{
		auto const table_address = m_destination;

		if (table_address.is_valid())
		{
			m_emitter.new_table(
				table_address.local_address()
				);
		}

		for (auto element = expression.elements().begin(), end = expression.elements().end();
			element != end; ++element)
		{
			auto const &value = *element->second;

			try
			{
				if (table_address.is_valid())
				{
					source_range const key = element->first;

					temporary const key_variable(
						m_frame,
						1
						);

					{
						auto const key_string_id = m_function_generator.unit().get_string_id(
							source_range_to_string(key)
							);

						m_emitter.set_string(
							key_variable.address().local_address(),
							key_string_id
							);
					}

					temporary const value_variable(
						m_frame,
						1
						);

					rvalue_generator value_generator(
						m_function_generator,
						m_emitter,
						m_frame,
						value_variable.address()
						);
					value.accept(value_generator);

					m_emitter.set_element(
						table_address.local_address(),
						key_variable.address().local_address(),
						value_variable.address().local_address()
						);
				}
				else
				{
					rvalue_generator value_generator(
						m_function_generator,
						m_emitter,
						m_frame,
						reference() //ignore value
						);
					value.accept(value_generator);
				}
			}
			catch (compiler_error const &e)
			{
				m_function_generator.handle_error(e);
			}
		}
	}

	void rvalue_generator::visit(unary_expression_tree const &expression)
	{
		rvalue_generator input_generator(
			m_function_generator,
			m_emitter,
			m_frame,
			m_destination
			);
		expression.input().accept(input_generator);

		if (m_destination.is_valid())
		{
			switch (expression.type())
			{
			case unary_operator::not_:
				m_emitter.not_(m_destination.local_address());
				break;

			case unary_operator::inverse:
				m_emitter.invert(m_destination.local_address());
				break;

			case unary_operator::negative:
				m_emitter.negate(m_destination.local_address());
				break;
			}
		}
	}

	namespace
	{
		intermediate::instruction_type::Enum binary_operation_to_instruction(
			binary_operator::Enum op)
		{
			using namespace intermediate;

			BOOST_STATIC_ASSERT(binary_operator::count_ == 19);
			switch (op)
			{
			case binary_operator::add: return instruction_type::add;
			case binary_operator::sub: return instruction_type::sub;
			case binary_operator::mul: return instruction_type::mul;
			case binary_operator::div: return instruction_type::div;
			case binary_operator::mod: return instruction_type::mod;
			case binary_operator::bit_and: return instruction_type::and_;
			case binary_operator::bit_or: return instruction_type::or_;
			case binary_operator::bit_xor: return instruction_type::xor_;
			case binary_operator::shift_left: return instruction_type::shift_left;
			case binary_operator::shift_right: return instruction_type::shift_right;
			case binary_operator::shift_signed: return instruction_type::shift_signed;
			case binary_operator::equal: return instruction_type::equal;
			case binary_operator::not_equal: return instruction_type::not_equal;
			case binary_operator::less: return instruction_type::less;
			case binary_operator::less_equal: return instruction_type::less_equal;
			case binary_operator::greater: return instruction_type::greater;
			case binary_operator::greater_equal: return instruction_type::greater_equal;
			default: assert(nullptr == "invalid binary operator id for direct translation"); return instruction_type::nothing;
			}
		}
	}

	void rvalue_generator::visit(binary_expression_tree const &expression)
	{
		bool const is_result_stored = m_destination.is_valid();

		switch (expression.type())
		{
		case binary_operator::logical_and:
			{
				temporary const left_variable(
					m_frame,
					m_destination.is_valid() ? 0 : 1
					);

				auto const left_address =
					(m_destination.is_valid() ? m_destination : left_variable.address());

				rvalue_generator left_generator(
					m_function_generator,
					m_emitter,
					m_frame,
					left_address
					);
				expression.left().accept(left_generator);

				auto const skip_right_address = m_emitter.get_current_jump_address();
				m_emitter.jump_if_not(
					-1,
					left_address.local_address()
					);

				rvalue_generator right_generator(
					m_function_generator,
					m_emitter,
					m_frame,
					m_destination
					);
				expression.right().accept(right_generator);

				m_emitter.update_jump_destination(
					skip_right_address,
					m_emitter.get_current_jump_address()
					);
				break;
			}

		case binary_operator::logical_or:
			break; //TODO

		default:
			{
				temporary const left_variable(
					m_frame,
					is_result_stored ? 1 : 0);

				{
					rvalue_generator left_generator(
						m_function_generator,
						m_emitter,
						m_frame,
						is_result_stored ? left_variable.address() : reference());
					expression.left().accept(left_generator);
				}

				temporary const right_variable(
					m_frame,
					is_result_stored ? 1 : 0);

				{
					rvalue_generator right_generator(
						m_function_generator,
						m_emitter,
						m_frame,
						is_result_stored ? right_variable.address() : reference());
					expression.right().accept(right_generator);
				}

				auto const instruction = binary_operation_to_instruction(expression.type());

				if (is_result_stored)
				{
					m_emitter.binary_operation(
						instruction,
						left_variable.address().local_address(),
						right_variable.address().local_address()
						);
					m_emitter.copy(
						m_destination.local_address(),
						left_variable.address().local_address()
						);
				}
				break;
			}
		}
	}

	namespace
	{
		void generate_subscript(
			local_frame &frame,
			intermediate::emitter &emitter,
			function_generator &function_generator,
			reference destination,
			expression_tree const &table_expression,
			std::function<void (reference &key_destination)> const &emit_key
			)
		{
			auto const table_address = destination;
			auto const value_address = destination;

			rvalue_generator table_generator(
				function_generator,
				emitter,
				frame,
				table_address
				);
			table_expression.accept(table_generator);

			temporary const key_variable(
				frame,
				1
				);

			auto key_address = key_variable.address();
			emit_key(key_address);

			if (destination.is_valid())
			{
				emitter.get_element(
					table_address.local_address(),
					key_address.local_address(),
					value_address.local_address()
					);
			}
		}
	}

	void rvalue_generator::visit(dot_element_expression_tree const &expression)
	{
		auto &function_generator = m_function_generator;
		auto &emitter = m_emitter;
		auto const element_name = expression.element_name();

		generate_subscript(
			m_frame,
			m_emitter,
			m_function_generator,
			m_destination,
			expression.table(),
			[&function_generator, &emitter, element_name](reference &key_destination)
		{
			auto key = source_range_to_string(element_name);
			auto const key_string_id = function_generator.unit().get_string_id(
				std::move(key)
				);

			emitter.set_string(
				key_destination.local_address(),
				key_string_id
				);
		});
	}

	void rvalue_generator::visit(subscript_expression_tree const &expression)
	{
		auto &function_generator = m_function_generator;
		auto &emitter = m_emitter;
		auto &frame = m_frame;
		auto const &key_expression = expression.key();

		generate_subscript(
			m_frame,
			m_emitter,
			m_function_generator,
			m_destination,
			expression.table(),
			[&function_generator, &emitter, &frame, &key_expression](reference &key_destination)
		{
			rvalue_generator key_generator(
				function_generator,
				emitter,
				frame,
				key_destination
				);
			key_expression.accept(key_generator);
		});
	}

	void rvalue_generator::visit(import_expression_tree const &expression)
	{
		temporary const result_variable(m_frame, m_destination.is_valid() ? 0 : 1);
		reference const result_ref = (m_destination.is_valid() ?
									  m_destination : result_variable.address());
		{
			rvalue_generator name_generator(
				m_function_generator,
				m_emitter,
				m_frame,
				result_ref);
			expression.name().accept(name_generator);
		}
		m_emitter.load_module(result_ref.local_address());
	}

	void rvalue_generator::visit(method_call_expression_tree const &expression)
	{
		temporary const method_name_variable(m_frame, 1);

		temporary const instance_variable(m_frame, 1);
		{
			rvalue_generator instance_generator(
						m_function_generator,
						m_emitter,
						m_frame,
						instance_variable.address());
			expression.instance().accept(instance_generator);
		}

		{
			auto const method_name_id = m_function_generator.unit().get_string_id(
						source_range_to_string(expression.method_name()));
			m_emitter.set_string(
						method_name_variable.address().local_address(),
						method_name_id);
		}

		auto const result_address = instance_variable.address().local_address();

		with_arguments(expression.arguments(),
					   [&]()
		{
			m_emitter.call_method(
						result_address,
						expression.arguments().size(),
						method_name_variable.address().local_address()
						);

			if (m_destination.is_valid())
			{
				m_emitter.copy(
					m_destination.local_address(),
					result_address
					);
			}
		});
	}
}

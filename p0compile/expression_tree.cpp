#include "expression_tree.hpp"
#include "statement_tree.hpp"


namespace p0
{
	expression_tree_visitor::~expression_tree_visitor()
	{
	}


	expression_tree::~expression_tree()
	{
	}


	name_expression_tree::name_expression_tree(
		source_range name
		)
		: m_name(name)
	{
	}

	source_range const &name_expression_tree::name() const
	{
		return m_name;
	}

	void name_expression_tree::accept(expression_tree_visitor &visitor) const
	{
		visitor.visit(*this);
	}

	source_range name_expression_tree::position() const
	{
		return m_name;
	}


	integer_10_expression_tree::integer_10_expression_tree(
		source_range value
		)
		: m_value(value)
	{
	}

	source_range const &integer_10_expression_tree::value() const
	{
		return m_value;
	}

	void integer_10_expression_tree::accept(expression_tree_visitor &visitor) const
	{
		visitor.visit(*this);
	}

	source_range integer_10_expression_tree::position() const
	{
		return m_value;
	}


	string_expression_tree::string_expression_tree(
		part_vector parts
		)
		: m_parts(std::move(parts))
	{
	}

	void string_expression_tree::accept(expression_tree_visitor &visitor) const
	{
		visitor.visit(*this);
	}

	source_range string_expression_tree::position() const
	{
		return source_range(
			m_parts.front().begin(),
			m_parts.back().end()
			);
	}

	string_expression_tree::part_vector const &string_expression_tree::parts() const
	{
		return m_parts;
	}


	call_expression_tree::call_expression_tree(
		std::unique_ptr<expression_tree> function,
		expression_vector arguments
		)
		: m_function(std::move(function))
		, m_arguments(std::move(arguments))
	{
	}

	void call_expression_tree::accept(expression_tree_visitor &visitor) const
	{
		visitor.visit(*this);
	}

	expression_tree const &call_expression_tree::function() const
	{
		return *m_function;
	}

	call_expression_tree::expression_vector const &call_expression_tree::arguments() const
	{
		return m_arguments;
	}

	source_range call_expression_tree::position() const
	{
		return m_function->position(); //TODO: the full call expression
	}


	function_tree::function_tree(
		std::unique_ptr<statement_tree> body,
		name_vector parameters,
		source_range position
		)
		: m_body(std::move(body))
		, m_parameters(std::move(parameters))
		, m_position(position)
	{
	}

	function_tree::~function_tree()
	{
	}

	void function_tree::accept(expression_tree_visitor &visitor) const
	{
		visitor.visit(*this);
	}

	statement_tree const &function_tree::body() const
	{
		return *m_body;
	}

	function_tree::name_vector const &function_tree::parameters() const
	{
		return m_parameters;
	}
	
	source_range function_tree::position() const
	{
		return m_position;
	}


	null_expression_tree::null_expression_tree(
		source_range position
		)
		: m_position(position)
	{
	}

	void null_expression_tree::accept(expression_tree_visitor &visitor) const
	{
		visitor.visit(*this);
	}

	source_range null_expression_tree::position() const
	{
		return m_position;
	}


	table_expression_tree::table_expression_tree(
		element_vector elements,
		source_range position
		)
		: m_elements(std::move(elements))
		, m_position(position)
	{
	}

	void table_expression_tree::accept(expression_tree_visitor &visitor) const
	{
		visitor.visit(*this);
	}

	source_range table_expression_tree::position() const
	{
		return m_position;
	}

	table_expression_tree::element_vector const &table_expression_tree::elements() const
	{
		return m_elements;
	}


	unary_expression_tree::unary_expression_tree(
		unary_operator::Enum type,
		std::unique_ptr<expression_tree> input,
		source_range position
		)
		: m_type(type)
		, m_input(std::move(input))
		, m_position(position)
	{
		assert(m_input);
	}

	void unary_expression_tree::accept(expression_tree_visitor &visitor) const
	{
		visitor.visit(*this);
	}

	source_range unary_expression_tree::position() const
	{
		return m_position;
	}

	unary_operator::Enum unary_expression_tree::type() const
	{
		return m_type;
	}

	expression_tree const &unary_expression_tree::input() const
	{
		return *m_input;
	}


	binary_expression_tree::binary_expression_tree(
		binary_operator::Enum type,
		std::unique_ptr<expression_tree> left,
		std::unique_ptr<expression_tree> right,
		source_range position
		)
		: m_type(type)
		, m_left(std::move(left))
		, m_right(std::move(right))
		, m_position(position)
	{
	}

	void binary_expression_tree::accept(expression_tree_visitor &visitor) const
	{
		visitor.visit(*this);
	}

	source_range binary_expression_tree::position() const
	{
		return m_position;
	}

	binary_operator::Enum binary_expression_tree::type() const
	{
		return m_type;
	}

	expression_tree const &binary_expression_tree::left() const
	{
		return *m_left;
	}

	expression_tree const &binary_expression_tree::right() const
	{
		return *m_right;
	}


	dot_element_expression_tree::dot_element_expression_tree(
		std::unique_ptr<expression_tree> table,
		source_range element_name
		)
		: m_table(std::move(table))
		, m_element_name(element_name)
	{
		assert(m_table);
	}

	void dot_element_expression_tree::accept(expression_tree_visitor &visitor) const
	{
		return visitor.visit(*this);
	}

	source_range dot_element_expression_tree::position() const
	{
		return m_table->position(); //TODO: full expression
	}

	expression_tree const &dot_element_expression_tree::table() const
	{
		return *m_table;
	}

	source_range const &dot_element_expression_tree::element_name() const
	{
		return m_element_name;
	}
}

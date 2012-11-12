#include "statement_tree.hpp"
#include "expression_tree.hpp"


namespace p0
{
	statement_tree_visitor::~statement_tree_visitor()
	{
	}


	statement_tree::~statement_tree()
	{
	}


	declaration_tree::declaration_tree(
		source_range name,
		std::unique_ptr<expression_tree> value
		)
		: m_name(std::move(name))
		, m_value(std::move(value))
	{
	}

	declaration_tree::~declaration_tree()
	{
	}

	void declaration_tree::accept(statement_tree_visitor &visitor) const
	{
		visitor.visit(*this);
	}

	source_range const &declaration_tree::name() const
	{
		return m_name;
	}

	expression_tree const &declaration_tree::value() const
	{
		return *m_value;
	}


	return_tree::return_tree(
		std::unique_ptr<expression_tree> value
		)
		: m_value(std::move(value))
	{
	}

	return_tree::~return_tree()
	{
	}

	void return_tree::accept(statement_tree_visitor &visitor) const
	{
		visitor.visit(*this);
	}

	expression_tree const &return_tree::value() const
	{
		return *m_value;
	}


	block_tree::block_tree(
		statement_vector body
		)
		: m_body(std::move(body))
	{
	}

	void block_tree::accept(statement_tree_visitor &visitor) const
	{
		visitor.visit(*this);
	}

	block_tree::statement_vector const &block_tree::body() const
	{
		return m_body;
	}


	expression_statement_tree::expression_statement_tree(
		std::unique_ptr<expression_tree> expression
		)
		: m_expression(std::move(expression))
	{
	}

	expression_statement_tree::~expression_statement_tree()
	{
	}

	void expression_statement_tree::accept(statement_tree_visitor &visitor) const
	{
		visitor.visit(*this);
	}

	expression_tree const &expression_statement_tree::expression() const
	{
		return *m_expression;
	}


	assignment_tree::assignment_tree(
		std::unique_ptr<expression_tree> destination,
		std::unique_ptr<expression_tree> source
		)
		: m_destination(std::move(destination))
		, m_source(std::move(source))
	{
	}

	void assignment_tree::accept(statement_tree_visitor &visitor) const
	{
		visitor.visit(*this);
	}

	expression_tree const &assignment_tree::destination() const
	{
		return *m_destination;
	}

	expression_tree const &assignment_tree::source() const
	{
		return *m_source;
	}


	if_tree::if_tree(
		std::unique_ptr<expression_tree> condition,
		std::unique_ptr<statement_tree> on_true,
		std::unique_ptr<statement_tree> on_false
		)
		: m_condition(std::move(condition))
		, m_on_true(std::move(on_true))
		, m_on_false(std::move(on_false))
	{
	}

	void if_tree::accept(statement_tree_visitor &visitor) const
	{
		visitor.visit(*this);
	}

	expression_tree const &if_tree::condition() const
	{
		return *m_condition;
	}

	statement_tree const &if_tree::on_true() const
	{
		return *m_on_true;
	}

	statement_tree const *if_tree::on_false() const
	{
		return m_on_false.get();
	}


	while_tree::while_tree(
		std::unique_ptr<expression_tree> condition,
		std::unique_ptr<statement_tree> body
		)
		: m_condition(std::move(condition))
		, m_body(std::move(body))
	{
	}

	void while_tree::accept(statement_tree_visitor &visitor) const
	{
		visitor.visit(*this);
	}

	expression_tree const &while_tree::condition() const
	{
		return *m_condition;
	}

	statement_tree const &while_tree::body() const
	{
		return *m_body;
	}


	void break_tree::accept(statement_tree_visitor &visitor) const
	{
		visitor.visit(*this);
	}


	void continue_tree::accept(statement_tree_visitor &visitor) const
	{
		visitor.visit(*this);
	}
}

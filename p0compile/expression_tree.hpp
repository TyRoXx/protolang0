#pragma once
#ifndef P0C_EXPRESSION_TREE_HPP
#define P0C_EXPRESSION_TREE_HPP


#include "source_range.hpp"
#include <memory>
#include <string>
#include <vector>


namespace p0
{
	struct name_expression_tree;
	struct integer_10_expression_tree;
	struct string_expression_tree;
	struct call_expression_tree;
	struct function_tree;
	struct null_expression_tree;
	struct table_expression_tree;
	struct unary_expression_tree;
	struct binary_expression_tree;
	struct dot_element_expression_tree;


	struct expression_tree_visitor
	{
		virtual ~expression_tree_visitor();
		virtual void visit(name_expression_tree const &expression) = 0;
		virtual void visit(integer_10_expression_tree const &expression) = 0;
		virtual void visit(string_expression_tree const &expression) = 0;
		virtual void visit(call_expression_tree const &expression) = 0;
		virtual void visit(function_tree const &expression) = 0;
		virtual void visit(null_expression_tree const &expression) = 0;
		virtual void visit(table_expression_tree const &expression) = 0;
		virtual void visit(unary_expression_tree const &expression) = 0;
		virtual void visit(binary_expression_tree const &expression) = 0;
		virtual void visit(dot_element_expression_tree const &expression) = 0;
	};


	struct expression_tree
	{
		virtual ~expression_tree();
		virtual void accept(expression_tree_visitor &visitor) const = 0;
		virtual source_range position() const = 0;
	};


	struct name_expression_tree : expression_tree
	{
		explicit name_expression_tree(
			source_range name
			);
		virtual void accept(expression_tree_visitor &visitor) const override;
		virtual source_range position() const override;
		source_range const &name() const;

	private:

		source_range m_name;
	};


	struct integer_10_expression_tree : expression_tree
	{
		explicit integer_10_expression_tree(
			source_range value
			);
		virtual void accept(expression_tree_visitor &visitor) const override;
		virtual source_range position() const override;
		source_range const &value() const;

	private:

		source_range m_value;
	};


	struct string_expression_tree : expression_tree
	{
		typedef std::vector<source_range> part_vector;


		explicit string_expression_tree(
			part_vector parts
			);
		virtual void accept(expression_tree_visitor &visitor) const override;
		virtual source_range position() const override;
		part_vector const &parts() const;

	private:

		part_vector m_parts;
	};


	struct call_expression_tree : expression_tree
	{
		typedef std::vector<std::unique_ptr<expression_tree>> expression_vector;


		explicit call_expression_tree(
			std::unique_ptr<expression_tree> function,
			expression_vector arguments
			);
		virtual void accept(expression_tree_visitor &visitor) const override;
		virtual source_range position() const override;
		expression_tree const &function() const;
		expression_vector const &arguments() const;

	private:

		std::unique_ptr<expression_tree> m_function;
		expression_vector m_arguments;
	};


	struct statement_tree;


	struct function_tree : expression_tree
	{
		typedef std::vector<source_range> name_vector;


		explicit function_tree(
			std::unique_ptr<statement_tree> body,
			name_vector parameters,
			source_range position
			);
		~function_tree();
		virtual void accept(expression_tree_visitor &visitor) const override;
		virtual source_range position() const override;
		statement_tree const &body() const;
		name_vector const &parameters() const;

	private:

		std::unique_ptr<statement_tree> m_body;
		name_vector const m_parameters;
		source_range const m_position;
	};


	struct null_expression_tree : expression_tree
	{
		explicit null_expression_tree(
			source_range position
			);
		virtual void accept(expression_tree_visitor &visitor) const override;
		virtual source_range position() const override;

	private:

		source_range m_position;
	};


	struct table_expression_tree : expression_tree
	{
		typedef std::pair<source_range, std::unique_ptr<expression_tree>> element;
		typedef std::vector<element> element_vector;


		explicit table_expression_tree(
			element_vector elements,
			source_range position
			);
		virtual void accept(expression_tree_visitor &visitor) const override;
		virtual source_range position() const override;
		element_vector const &elements() const;

	private:

		element_vector const m_elements;
		source_range const m_position;
	};


	namespace unary_operator
	{
		enum Enum
		{
			not_,
			inverse,
			negative,
		};
	};

	struct unary_expression_tree : expression_tree
	{
		explicit unary_expression_tree(
			unary_operator::Enum type,
			std::unique_ptr<expression_tree> input,
			source_range position
			);
		virtual void accept(expression_tree_visitor &visitor) const override;
		virtual source_range position() const override;
		unary_operator::Enum type() const;
		expression_tree const &input() const;

	private:

		unary_operator::Enum const m_type;
		std::unique_ptr<expression_tree> m_input;
		source_range const m_position;
	};


	namespace binary_operator
	{
		enum Enum
		{
			add,
			sub,
			mul,
			div,
			mod,
			bit_and,
			bit_or,
			bit_xor,
			shift_left,
			shift_right,
			equal,
			not_equal,
			less,
			less_equal,
			greater,
			greater_equal,
			logical_and,
			logical_or,
		};
	};

	struct binary_expression_tree : expression_tree
	{
		explicit binary_expression_tree(
			binary_operator::Enum type,
			std::unique_ptr<expression_tree> left,
			std::unique_ptr<expression_tree> right,
			source_range position
			);
		virtual void accept(expression_tree_visitor &visitor) const override;
		virtual source_range position() const override;
		binary_operator::Enum type() const;
		expression_tree const &left() const;
		expression_tree const &right() const;

	private:

		binary_operator::Enum m_type;
		std::unique_ptr<expression_tree> m_left, m_right;
		source_range const m_position;
	};


	struct dot_element_expression_tree : expression_tree
	{
		explicit dot_element_expression_tree(
			std::unique_ptr<expression_tree> table,
			source_range element_name
			);
		virtual void accept(expression_tree_visitor &visitor) const override;
		virtual source_range position() const override;
		expression_tree const &table() const;
		source_range const &element_name() const;

	private:

		std::unique_ptr<expression_tree> m_table;
		source_range m_element_name;
	};
}


#endif

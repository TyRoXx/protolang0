#include "code_generator.hpp"
#include "expression_tree.hpp"
#include "statement_tree.hpp"
#include "local_frame.hpp"
#include "compiler_error.hpp"
#include "statement_code_generator.hpp"
#include "p0i/emitter.hpp"


namespace p0
{
	code_generator::code_generator(
		function_tree const &tree,
		size_t integer_width,
		compiler_error_handler error_handler
		)
		: m_tree(tree)
		, m_integer_width(integer_width)
		, m_error_handler(std::move(error_handler))
	{
		assert(integer_width >= 1);
		assert(integer_width <= std::numeric_limits<intermediate::instruction_argument>::digits);
	}

	size_t code_generator::integer_width() const
	{
		return m_integer_width;
	}

	intermediate::unit code_generator::generate_unit()
	{
		generate_function(
			m_tree
			);

		intermediate::unit::string_vector strings(
			m_string_ids.size()
			);

		for (auto s = m_string_ids.begin(); s != m_string_ids.end(); ++s)
		{
			auto const id = s->second;
			auto value = s->first;

			strings[id] = std::move(value);
		}

		return intermediate::unit(
			std::move(m_functions),
			std::move(strings),
			m_integer_width
			);
	}

	size_t code_generator::generate_function(
		function_tree const &function
		)
	{
		//reserve a function index for later insertion
		auto const function_index = m_functions.size();
		m_functions.resize(function_index + 1);

		intermediate::function::instruction_vector instructions;
		intermediate::emitter emitter(instructions);

		local_frame top_frame(
			nullptr
			);
		for (auto p = begin(function.parameters()), e = end(function.parameters()); p != e; ++p)
		{
			try
			{
				top_frame.declare_variable(*p);
			}
			catch (compiler_error const &e)
			{
				m_error_handler(e);
			}
		}

		generate_statement(
			function.body(),
			*this,
			emitter,
			top_frame
			);
		
		m_functions[function_index] = intermediate::function(
			std::move(instructions),
			function.parameters().size()
			);

		return function_index;
	}

	void code_generator::handle_error(
		compiler_error const &error
		)
	{
		m_error_handler(error);
	}

	size_t code_generator::get_string_id(
		std::string value
		)
	{
		auto const pos = m_string_ids.find(value);
		if (pos == m_string_ids.end())
		{
			auto const id = m_string_ids.size();
			m_string_ids.insert(std::make_pair(
				std::move(value),
				id
				));
			return id;
		}

		return pos->second;
	}
}

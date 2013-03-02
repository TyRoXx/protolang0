#include "unit_generator.hpp"
#include "function_generator.hpp"
#include <cassert>


namespace p0
{
	unit_generator::unit_generator(
		function_tree const &tree,
		compiler_error_handler error_handler
		)
		: m_tree(tree)
		, m_error_handler(std::move(error_handler))
	{
	}

	function_tree const &unit_generator::tree() const
	{
		return m_tree;
	}

	unit_generator::compiler_error_handler const &unit_generator::error_handler() const
	{
		return m_error_handler;
	}

	intermediate::unit unit_generator::generate_unit()
	{
		function_generator function_generator(*this);
		function_generator.generate_function(
			m_tree,
			nullptr
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
			std::move(strings)
			);
	}
	
	void unit_generator::handle_error(
		compiler_error const &error
		)
	{
		m_error_handler(error);
	}

	size_t unit_generator::get_string_id(
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

	std::size_t unit_generator::allocate_function()
	{
		auto const id = m_functions.size();
		m_functions.resize(id + 1);
		return id;
	}

	void unit_generator::define_function(std::size_t id, intermediate::function definition)
	{
		assert(id < m_functions.size());
		m_functions[id] = std::move(definition);
	}
}

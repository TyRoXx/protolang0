#include "local_frame.hpp"
#include "compiler_error.hpp"


namespace p0
{
	local_frame::local_frame(
		local_frame const *parent
		)
		: m_parent(parent)
		, m_next_local_address(parent ? parent->m_next_local_address : 0)
	{
	}

	reference local_frame::declare_variable(
		source_range name
		)
	{
		//non-const for moving later
		std::string name_str = source_range_to_string(name);

		auto const s = m_symbols_by_name.find(name_str);
		if (s != m_symbols_by_name.end())
		{
			throw compiler_error(
				"Name of local variable is already in use",
				name
				);
		}

		reference const variable_address(m_next_local_address);

		m_symbols_by_name.insert(
			s,
			std::make_pair(std::move(name_str), variable_address)
			);

		++m_next_local_address;
		return variable_address;
	}

	reference local_frame::require_symbol(
		source_range name
		) const
	{
		auto const s = m_symbols_by_name.find(
			source_range_to_string(name)
			);

		if (s != m_symbols_by_name.end())
		{
			return s->second;
		}

		if (m_parent)
		{
			return m_parent->require_symbol(name);
		}

		throw compiler_error(
			"Unknown identifier",
			name
			);
	}

	reference local_frame::allocate(size_t count)
	{
		reference const result(m_next_local_address);
		m_next_local_address += count;
		return result;
	}

	void local_frame::deallocate_top(size_t count)
	{
		assert(m_next_local_address >= count);
		m_next_local_address -= count;
	}
}

#include "local_frame.hpp"
#include "compiler_error.hpp"
#include "function_generator.hpp"
#include "p0i/emitter.hpp"
#include <boost/foreach.hpp>


namespace p0
{
	local_frame::local_frame(
		local_frame &parent
		)
		: m_parent(&parent)
		, m_function_generator(parent.m_function_generator)
	{
		init_variables();
	}

	local_frame::local_frame(
		function_generator &function_generator
		)
		: m_parent(nullptr)
		, m_function_generator(function_generator)
	{
		init_variables();
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

	reference local_frame::require_writeable(
		std::string const &name,
		source_range name_position
		)
	{
		//TODO
		return require_symbol(name_position);
	}

	reference local_frame::emit_read_only(
		std::string const &name,
		source_range name_position,
		reference possible_space
		)
	{
		//TODO
		return require_symbol(name_position);
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

	loop *local_frame::enter_loop(loop &loop)
	{
		auto * const previous = m_current_loop;
		m_current_loop = &loop;
		return previous;
	}

	void local_frame::leave_loop(loop *previous)
	{
		assert(m_current_loop);
		m_current_loop = previous;
	}

	loop *local_frame::get_loop() const
	{
		return m_current_loop;
	}

	void local_frame::init_variables()
	{
		m_next_local_address = (m_parent ? m_parent->m_next_local_address : 0);
		m_current_loop = (m_parent ? m_parent->m_current_loop : nullptr);
	}


	loop::loop(local_frame &frame,
			   intermediate::emitter &emitter,
			   std::size_t continue_destination)
		: m_frame(frame)
		, m_emitter(emitter)
		, m_previous(frame.enter_loop(*this))
		, m_continue_destination(continue_destination)
	{
	}

	loop::~loop()
	{
		m_frame.leave_loop(m_previous);
	}

	void loop::emit_break()
	{
		auto const break_address = m_emitter.get_current_jump_address();
		m_emitter.jump(-1);
		m_breaks.push_back(break_address);
	}

	void loop::emit_continue()
	{
		m_emitter.jump(m_continue_destination);
	}

	void loop::finish(std::size_t after_loop)
	{
		BOOST_FOREACH (auto const break_address, m_breaks)
		{
			m_emitter.update_jump_destination(
						break_address,
						after_loop);
		}
	}
}

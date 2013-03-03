#include "local_frame.hpp"
#include "compiler_error.hpp"
#include "function_generator.hpp"
#include "temporary.hpp"
#include "p0i/emitter.hpp"


namespace p0
{
	local_frame::local_frame(
		local_frame &function_parent
		)
		: m_function_parent(&function_parent)
		, m_function_generator(function_parent.m_function_generator)
	{
		init_variables();
	}

	local_frame::local_frame(
		function_generator &function_generator
		)
		: m_function_parent(nullptr)
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

	reference local_frame::require_writeable(
		std::string const &name,
		source_range name_position
		)
	{
		auto const result = find_function_local_variable(name);
		if (result.is_valid())
		{
			return result;
		}

		throw compiler_error(
			"Unknown identifier",
			name_position
			);
	}

	reference local_frame::emit_read_only(
		std::string const &name,
		source_range name_position,
		reference possible_space,
		intermediate::emitter &emitter
		)
	{
		//trivial case:
		//The variable is function-local therefore direcly accessible.
		auto const function_local = find_function_local_variable(name);
		if (function_local.is_valid())
		{
			return function_local;
		}

		//TODO: look for the variable in outer function

		std::vector<function_generator *> frames;
		size_t current_bound_index = static_cast<size_t>(-1);

		local_frame *outer_frame = this;
		for (;;)
		{
			outer_frame = outer_frame->outer_function_inner_frame();
			if (!outer_frame)
			{
				throw compiler_error(
					"Unknown identifier",
					name_position
					);
			}

			auto &current_function = outer_frame->m_function_generator;

			auto const local =
				outer_frame->find_function_local_variable(name);
			if (local.is_valid())
			{
				auto const outmost_bound_index =
					current_function.bind_local(local);

				current_bound_index = outmost_bound_index;
				break;
			}

			frames.push_back(&current_function);
		}

		assert(current_bound_index != static_cast<size_t>(-1));

		for (auto f = frames.begin(); f != frames.end(); ++f)
		{
			auto &inner_function = **f;

			current_bound_index = inner_function.bind_from_parent(
				current_bound_index
				);
		}

		if (possible_space.is_valid())
		{
			temporary const current_function_variable(*this, 1);

			emitter.current_function(
				current_function_variable.address().local_address()
				);
			emitter.get_bound(
				current_function_variable.address().local_address(),
				current_bound_index,
				possible_space.local_address()
				);
		}
		return possible_space;
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


	local_frame *local_frame::outer_function_inner_frame() const
	{
		return m_function_generator.outer_frame();
	}

	void local_frame::init_variables()
	{
		if (m_function_parent)
		{
			m_next_local_address = m_function_parent->m_next_local_address;
			m_current_loop = m_function_parent->m_current_loop;
		}
		else
		{
			m_next_local_address = 0;
			m_current_loop = nullptr;
		}
	}

	reference local_frame::find_function_local_variable(
			std::string const &name
			)
	{
		{
			auto const symbol = m_symbols_by_name.find(name);
			if (symbol != m_symbols_by_name.end())
			{
				return symbol->second;
			}
		}

		if (m_function_parent)
		{
			return m_function_parent->find_function_local_variable(name);
		}

		return reference();
	}
}

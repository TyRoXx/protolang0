#include "function_generator.hpp"
#include "unit_generator.hpp"
#include "local_frame.hpp"
#include "expression_tree.hpp"
#include "compiler_error.hpp"
#include "temporary.hpp"
#include "statement_code_generator.hpp"
#include "p0i/emitter.hpp"
#include "p0common/integer_cast.hpp"
#include <cassert>


namespace p0
{
	function_generator::function_generator(
		unit_generator &unit
		)
		: m_unit(unit)
		, m_outer_frame(nullptr)
	{
	}

	function_generator::function_generator(
		function_generator &parent,
		local_frame *outer_frame
		)
		: m_unit(parent.m_unit)
		, m_outer_frame(outer_frame)
	{
	}

	unit_generator &function_generator::unit() const
	{
		return m_unit;
	}

	local_frame *function_generator::outer_frame() const
	{
		return m_outer_frame;
	}

	size_t function_generator::generate_function(
		function_tree const &function
		)
	{
		assert(m_return_instructions.empty());

		//reserve a function index for later insertion
		auto const function_index = m_unit.allocate_function();

		intermediate::function::instruction_vector instructions;
		intermediate::emitter emitter(instructions);

		local_frame top_frame(*this);

		//return value
		top_frame.allocate(1);

		for (auto p = begin(function.parameters()), e = end(function.parameters()); p != e; ++p)
		{
			try
			{
				top_frame.declare_variable(*p);
			}
			catch (compiler_error const &e)
			{
				handle_error(e);
			}
		}

		generate_statement(
			function.body(),
			*this,
			emitter,
			top_frame
			);

		for (auto i = m_return_instructions.begin();
			i != m_return_instructions.end(); ++i)
		{
			auto &return_instruction = instructions[*i];
			assert(intermediate::is_any_jump(return_instruction.type()));
			return_instruction.arguments()[0] =
					integer_cast<instruction_argument>(instructions.size());
		}

		m_unit.define_function(function_index, intermediate::function(
			std::move(instructions),
			function.parameters().size(),
			m_bound_variables.size()
			));

		return function_index;
	}

	void function_generator::handle_error(
		compiler_error const &error
		)
	{
		m_unit.handle_error(error);
	}

	void function_generator::add_return(jump_offset jump_address)
	{
		m_return_instructions.push_back(jump_address);
	}

	std::size_t function_generator::bind_local(reference bound_variable)
	{
		return bind(bound_variable);
	}

	std::size_t function_generator::bind_from_parent(size_t index_in_parent)
	{
		return bind(index_in_parent);
	}

	struct function_generator::bound_variable_emitter
		: boost::static_visitor<void>
	{
		bound_variable_emitter(
			intermediate::emitter &emitter,
			local_frame &frame,
			local_address closure_address,
			size_t bound_index,
			std::unique_ptr<temporary> &current_function)
			: m_emitter(emitter)
			, m_frame(frame)
			, m_closure_address(closure_address)
			, m_bound_index(bound_index)
			, m_current_function(current_function)
		{
		}

		void operator ()(function_generator::bound_from_parent variable) const
		{
			auto const current_function = require_current_function();
			temporary const buffer(m_frame, 1);

			m_emitter.get_bound(
						current_function,
						variable,
						buffer.address().local_address());

			m_emitter.bind(m_closure_address,
						   m_bound_index,
						   buffer.address().local_address());
		}

		void operator ()(function_generator::bound_locally variable) const
		{
			assert(variable.is_valid());
			m_emitter.bind(m_closure_address,
						   m_bound_index,
						   variable.local_address());
		}

	private:

		intermediate::emitter &m_emitter;
		local_frame &m_frame;
		local_address const m_closure_address;
		size_t const m_bound_index;
		std::unique_ptr<temporary> &m_current_function;


		local_address require_current_function() const
		{
			if (!m_current_function)
			{
				m_current_function.reset(
					new temporary(m_frame, 1));

				m_emitter.current_function(
					m_current_function->address().local_address());
			}
			return m_current_function->address().local_address();
		}
	};

	void function_generator::emit_bindings(
		local_address closure_address,
		local_frame &frame,
		intermediate::emitter &emitter) const
	{
		std::unique_ptr<temporary> current_function;

		for (size_t i = 0; i < m_bound_variables.size(); ++i)
		{
			auto &variable = m_bound_variables[i];
			bound_variable_emitter const visitor(
				emitter,
				frame,
				closure_address,
				i,
				current_function
				);
			boost::apply_visitor(visitor, variable);
		}
	}


	std::size_t function_generator::bind(bound_variable const &bound)
	{
		auto const existing = std::find(
					begin(m_bound_variables),
					end(m_bound_variables),
					bound);
		if (existing == m_bound_variables.end())
		{
			auto const id = m_bound_variables.size();
			m_bound_variables.push_back(bound);
			return id;
		}

		return static_cast<std::size_t>(
					existing - begin(m_bound_variables));
	}
}

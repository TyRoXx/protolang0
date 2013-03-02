#include "function_generator.hpp"
#include "unit_generator.hpp"
#include "local_frame.hpp"
#include "expression_tree.hpp"
#include "compiler_error.hpp"
#include "statement_code_generator.hpp"
#include "p0i/emitter.hpp"
#include <cassert>


namespace p0
{
	function_generator::function_generator(
		unit_generator &unit
		)
		: m_unit(unit)
	{
	}

	unit_generator &function_generator::unit() const
	{
		return m_unit;
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

		local_frame top_frame(
			nullptr
			);

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
			return_instruction.arguments()[0] = instructions.size();
		}

		m_unit.define_function(function_index, intermediate::function(
			std::move(instructions),
			function.parameters().size()
			));

		return function_index;
	}

	void function_generator::handle_error(
		compiler_error const &error
		)
	{
		m_unit.handle_error(error);
	}

	void function_generator::add_return(std::size_t jump_address)
	{
		m_return_instructions.push_back(jump_address);
	}
}

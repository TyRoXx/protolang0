#include "compiler.hpp"
#include "scanner.hpp"
#include "parser.hpp"
#include "code_generator.hpp"

#include "expression_tree.hpp"


namespace p0
{
	compiler::compiler(
		source_range source,
		size_t integer_width,
		compiler_error_handler error_handler
		)
		: m_source(source)
		, m_integer_width(integer_width)
		, m_error_handler(std::move(error_handler))
	{
	}

	intermediate::unit compiler::compile()
	{
		scanner scanner(
			m_source
			);

		parser parser(
			scanner,
			m_error_handler
			);

		auto const tree = parser.parse_unit();

		code_generator generator(
			*tree,
			m_integer_width,
			m_error_handler
			);

		return generator.generate_unit();
	}
}

#pragma once
#ifndef P0C_COMPILER_HPP
#define P0C_COMPILER_HPP


#include "p0i/unit.hpp"
#include "source_range.hpp"
#include <functional>


namespace p0
{
	struct compiler_error;


	struct compiler
	{
		typedef std::function<bool (compiler_error const &)> compiler_error_handler;


		explicit compiler(
			source_range source,
			size_t integer_width,
			compiler_error_handler error_handler
			);
		intermediate::unit compile();

	private:

		source_range const m_source;
		size_t const m_integer_width;
		compiler_error_handler const m_error_handler;
	};
}


#endif

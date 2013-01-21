#pragma once
#ifndef P0RUN_INTERPRETER_HPP
#define P0RUN_INTERPRETER_HPP


#include "p0i/unit.hpp"
#include "value.hpp"


namespace p0
{
	namespace run
	{
		struct interpreter
		{
			explicit interpreter(
					intermediate::unit const &program);
			value call(
					intermediate::function const &function,
					const std::vector<value> &arguments);

		private:

			intermediate::unit const &m_program;
		};
	}
}


#endif

#pragma once
#ifndef P0RUN_INTERPRETER_HPP
#define P0RUN_INTERPRETER_HPP


#include "p0i/unit.hpp"


namespace p0
{
	namespace run
	{
		struct interpreter
		{
			explicit interpreter(
					intermediate::unit const &program);
			void call(
					intermediate::function const &function);

		private:

			intermediate::unit const &m_program;
		};
	}
}


#endif

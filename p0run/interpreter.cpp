#include "interpreter.hpp"
#include <cassert>


namespace p0
{
	namespace run
	{
		interpreter::interpreter(intermediate::unit const &program)
			: m_program(program)
		{
		}

		void interpreter::call(intermediate::function const &function)
		{
			assert(!"not implemented");
		}
	}
}

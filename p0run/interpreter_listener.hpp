#pragma once
#ifndef P0RUN_INTERPRETER_LISTENER_HPP
#define P0RUN_INTERPRETER_LISTENER_HPP


#include <cstddef>


namespace p0
{
	namespace intermediate
	{
		struct instruction;
	}

	namespace run
	{
		struct interpreter_listener
		{
			virtual ~interpreter_listener();
			virtual void enter_function(std::size_t arguments_address, std::size_t argument_count) = 0;
			virtual void leave_function() = 0;
			virtual void before_step(intermediate::instruction const &instruction) = 0;
		};
	}
}


#endif

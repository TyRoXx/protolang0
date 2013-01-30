#pragma once
#ifndef P0RUN_INTERPRETER_HPP
#define P0RUN_INTERPRETER_HPP


#include "p0i/unit.hpp"
#include "value.hpp"
#include "garbage_collector.hpp"


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
			void collect_garbage();

		private:

			intermediate::unit const &m_program;
			garbage_collector m_gc;
			std::vector<value> m_locals;


			void native_call(std::size_t arguments_address, std::size_t argument_count);
			value &get(std::size_t local_frame, std::size_t address);
		};
	}
}


#endif

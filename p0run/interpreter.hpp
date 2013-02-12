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
		struct interpreter_listener;


		struct interpreter
		{
			typedef std::function<std::unique_ptr<object> (std::string const &)>
				load_module_function;


			explicit interpreter(
					intermediate::unit const &program,
					load_module_function load_module);
			value call(
					intermediate::function const &function,
					const std::vector<value> &arguments);
			void collect_garbage();
			void set_listener(interpreter_listener *listener);

		private:

			intermediate::unit const &m_program;
			load_module_function const m_load_module;
			garbage_collector m_gc;
			std::vector<value> m_locals;
			interpreter_listener *m_listener;


			void native_call(std::size_t arguments_address, std::size_t argument_count);
			value &get(std::size_t local_frame, std::size_t address);
		};
	}
}


#endif

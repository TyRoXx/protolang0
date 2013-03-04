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


		struct interpreter PROTOLANG0_FINAL_CLASS
		{
			typedef std::function<value (interpreter &, std::string const &)>
				load_module_function;


			explicit interpreter(
					intermediate::unit const &program,
					load_module_function load_module);
			value call(
					intermediate::function const &function,
					value const &current_function,
					const std::vector<value> &arguments
					);
			value call(
				intermediate::function const &function,
				const std::vector<value> &arguments
				);
			void collect_garbage();
			void set_listener(interpreter_listener *listener);
			object &register_object(std::unique_ptr<object> object);

		private:

			intermediate::unit const &m_program;
			load_module_function const m_load_module;
			garbage_collector m_gc;
			std::vector<value> m_locals;
			std::vector<value> m_current_function_stack;
			interpreter_listener *m_listener;


			void native_call(
				intermediate::function const &function,
				std::size_t arguments_address,
				std::size_t argument_count
				);
			value &get(std::size_t local_frame, std::size_t address);
			static void mark_values(std::vector<value> const &values);
		};
	}
}


#endif

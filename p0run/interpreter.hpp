#pragma once
#ifndef P0RUN_INTERPRETER_HPP
#define P0RUN_INTERPRETER_HPP


#include "p0i/unit.hpp"
#include "value.hpp"
#include "garbage_collector.hpp"
#include <set>


namespace p0
{
	namespace run
	{
		struct interpreter_listener;


		struct interpreter PROTOLANG0_FINAL_CLASS
		{
			typedef std::function<value (interpreter &, std::string const &)>
				load_module_function;


			explicit interpreter(load_module_function load_module);
			value call(
				intermediate::function_ref const &function,
				value const &current_function,
				const std::vector<value> &arguments
				);
			value call(
				intermediate::function_ref const &function,
				const std::vector<value> &arguments
				);
			void collect_garbage();
			void set_listener(interpreter_listener *listener);
			object &register_object(std::unique_ptr<object> object);

		private:

			load_module_function const m_load_module;
			garbage_collector m_gc;
			std::vector<value> m_locals;
			std::vector<value> m_current_function_stack;
			interpreter_listener *m_listener;
			std::set<intermediate::function_ref> m_function_refs;


			void native_call(
				intermediate::function_ref const &function,
				std::size_t arguments_address,
				std::size_t argument_count
				);
			value &get(std::size_t local_frame, std::size_t address);
			intermediate::function_ref const &require_function_ref(
					intermediate::function_ref const &function);
			static void mark_values(std::vector<value> const &values);
		};
	}
}


#endif

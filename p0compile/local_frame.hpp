#pragma once
#ifndef P0C_LOCAL_FRAME_HPP
#define P0C_LOCAL_FRAME_HPP


#include "reference.hpp"
#include "source_range.hpp"
#include <map>
#include <string>
#include <vector>


namespace p0
{
	struct loop;
	struct function_generator;

	namespace intermediate
	{
		struct emitter;
	}


	struct local_frame PROTOLANG0_FINAL_CLASS
	{
		explicit local_frame(
			local_frame &parent
			);
		explicit local_frame(
			function_generator &function_generator
			);
		reference declare_variable(
			source_range name
			);
		reference require_symbol(
			source_range name
			) const;
		reference allocate(size_t count);
		void deallocate_top(size_t count);
		loop *enter_loop(loop &loop);
		void leave_loop(loop *previous);
		loop *get_loop() const;

	private:

		typedef std::map<std::string, reference> symbols_by_name;


		local_frame * const m_parent;
		function_generator &m_function_generator;
		symbols_by_name m_symbols_by_name;
		size_t m_next_local_address;
		loop *m_current_loop;


		void init_variables();
	};


	struct loop PROTOLANG0_FINAL_CLASS
	{
		explicit loop(local_frame &frame,
					  intermediate::emitter &emitter,
					  std::size_t continue_destination);
		~loop();
		void emit_break();
		void emit_continue();
		void finish(std::size_t after_loop);

	private:

		local_frame &m_frame;
		intermediate::emitter &m_emitter;
		loop * const m_previous;
		std::size_t const m_continue_destination;
		std::vector<std::size_t> m_breaks;
	};


}


#endif

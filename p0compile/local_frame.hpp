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

	namespace intermediate
	{
		struct emitter;
	}


	struct local_frame
	{
		explicit local_frame(
			local_frame const *parent
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


		local_frame const * const m_parent;
		symbols_by_name m_symbols_by_name;
		size_t m_next_local_address;
		loop *m_current_loop;
	};


	struct loop
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

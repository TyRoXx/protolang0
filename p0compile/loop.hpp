#pragma once
#ifndef P0C_LOOP_HPP
#define P0C_LOOP_HPP


#include "p0common/final.hpp"
#include "p0common/types.hpp"
#include <cstddef>
#include <vector>


namespace p0
{
	struct local_frame;

	namespace intermediate
	{
		struct emitter;
	}


	struct loop PROTOLANG0_FINAL_CLASS
	{
		explicit loop(local_frame &frame,
					  intermediate::emitter &emitter,
					  jump_offset continue_destination);
		~loop();
		void emit_break();
		void emit_continue();
		void finish(jump_offset after_loop);

	private:

		local_frame &m_frame;
		intermediate::emitter &m_emitter;
		loop * const m_previous;
		jump_offset const m_continue_destination;
		std::vector<jump_offset> m_breaks;
	};
}


#endif

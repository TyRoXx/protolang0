#pragma once
#ifndef P0C_LOOP_HPP
#define P0C_LOOP_HPP


#include "p0common/final.hpp"
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

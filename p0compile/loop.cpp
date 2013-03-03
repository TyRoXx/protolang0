#include "loop.hpp"
#include "local_frame.hpp"
#include "p0i/emitter.hpp"
#include <boost/foreach.hpp>


namespace p0
{
	loop::loop(local_frame &frame,
			   intermediate::emitter &emitter,
			   std::size_t continue_destination)
		: m_frame(frame)
		, m_emitter(emitter)
		, m_previous(frame.enter_loop(*this))
		, m_continue_destination(continue_destination)
	{
	}

	loop::~loop()
	{
		m_frame.leave_loop(m_previous);
	}

	void loop::emit_break()
	{
		auto const break_address = m_emitter.get_current_jump_address();
		m_emitter.jump(-1);
		m_breaks.push_back(break_address);
	}

	void loop::emit_continue()
	{
		m_emitter.jump(m_continue_destination);
	}

	void loop::finish(std::size_t after_loop)
	{
		BOOST_FOREACH (auto const break_address, m_breaks)
		{
			m_emitter.update_jump_destination(
						break_address,
						after_loop);
		}
	}
}

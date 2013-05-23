#include "temporary.hpp"
#include "local_frame.hpp"


namespace p0
{
	temporary::temporary(
		local_frame &frame,
		local_address size
		) PROTOLANG0_NOEXCEPT
		: frame(frame)
		, m_address(frame.allocate(size))
		, m_size(size)
	{
	}

	temporary::~temporary() PROTOLANG0_NOEXCEPT
	{
		frame.deallocate_top(m_size);
	}

	reference temporary::address() const PROTOLANG0_NOEXCEPT
	{
		return m_address;
	}

	local_address temporary::size() const PROTOLANG0_NOEXCEPT
	{
		return m_size;
	}
}

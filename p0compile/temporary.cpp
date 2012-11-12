#include "temporary.hpp"
#include "local_frame.hpp"


namespace p0
{
	temporary::temporary(
		local_frame &frame,
		size_t size
		)
		: frame(frame)
		, m_address(frame.allocate(size))
		, m_size(size)
	{
	}

	temporary::~temporary()
	{
		frame.deallocate_top(m_size);
	}

	reference temporary::address() const
	{
		return m_address;
	}

	size_t temporary::size() const
	{
		return m_size;
	}
}

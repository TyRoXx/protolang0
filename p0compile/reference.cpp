#include "reference.hpp"
#include <cassert>


namespace p0
{
	reference::reference()
		: m_local_address(static_cast<size_t>(-1))
	{
	}

	reference::reference(
		size_t local_address
		)
		: m_local_address(local_address)
	{
	}

	size_t reference::local_address() const
	{
		assert(is_valid());
		return m_local_address;
	}

	bool reference::is_valid() const
	{
		return (m_local_address != reference().m_local_address);
	}


	bool operator == (reference const &left, reference const &right)
	{
		return (left.local_address() == right.local_address());
	}
}

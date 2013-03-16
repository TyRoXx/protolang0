#include "raw_storage.hpp"
#include <cassert>


namespace p0
{
	namespace run
	{
		raw_storage::raw_storage()
			: m_gc(nullptr)
			, m_memory(nullptr)
		{
		}

		raw_storage::raw_storage(garbage_collector &gc, std::size_t byte_size)
			: m_gc(&gc)
			, m_memory(gc.allocate(byte_size))
		{
		}

		raw_storage::raw_storage(raw_storage &&other)
			: m_gc(nullptr)
			, m_memory(nullptr)
		{
			swap(other);
		}

		raw_storage::~raw_storage()
		{
			if (m_memory)
			{
				assert(m_gc);
				m_gc->deallocate(m_memory);
			}
		}

		raw_storage &raw_storage::operator = (raw_storage &&other)
		{
			swap(other);
			return *this;
		}

		void raw_storage::swap(raw_storage &other)
		{
			if (this != &other)
			{
				using std::swap;
				swap(m_gc, other.m_gc);
				swap(m_memory, other.m_memory);
			}
		}
	}
}

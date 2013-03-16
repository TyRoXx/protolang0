#pragma once
#ifndef P0RUN_RAW_STORAGE_HPP
#define P0RUN_RAW_STORAGE_HPP


#include "garbage_collector.hpp"
#include "p0common/final.hpp"
#include <cstddef>
#include <utility>


namespace p0
{
	namespace run
	{
		struct raw_storage PROTOLANG0_FINAL_CLASS
		{
			raw_storage();
			raw_storage(garbage_collector &gc, std::size_t byte_size);
			raw_storage(raw_storage &&other);
			~raw_storage();
			raw_storage &operator = (raw_storage &&other);
			void swap(raw_storage &other);

			template <class T, class ...Args>
			object &construct(Args && ...args)
			{
				object &constructed = *new (m_memory) T(std::forward<Args>(args)...);
				m_gc->commit_object(constructed);
				m_memory = nullptr;
				return constructed;
			}

		private:

			garbage_collector *m_gc;
			char *m_memory;
		};


		template <class T, class ...Args>
		object &construct_object(garbage_collector &gc, Args && ...args)
		{
			return raw_storage(gc, sizeof(T)).construct<T>(std::forward<Args>(args)...);
		}
	}
}


#endif

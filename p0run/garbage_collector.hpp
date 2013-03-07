#pragma once
#ifndef P0RUN_GARBAGE_COLLECTOR_HPP
#define P0RUN_GARBAGE_COLLECTOR_HPP


#include <stdexcept>


namespace p0
{
	namespace run
	{
		struct object;


		namespace sweep_mode
		{
			enum type
			{
				partial,
				full
			};
		}


		struct garbage_collector
		{
			virtual ~garbage_collector();
			virtual char *allocate(std::size_t byte_size) = 0;
			virtual void deallocate(char *storage) = 0;
			virtual void commit_object(object &constructed) = 0;
			virtual void unmark() = 0;
			virtual void sweep(sweep_mode::type mode) = 0;
		};


		struct raw_storage
		{
			raw_storage();
			raw_storage(garbage_collector &gc, char *memory);
			raw_storage(raw_storage &&other);
			~raw_storage();
			raw_storage &operator = (raw_storage &&other);
			void swap(raw_storage &other);

			template <class T, class ...Args>
			object &construct(Args ...args)
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


		template <class T>
		object &construct_object(garbage_collector &gc)
		{
			T * const ptr = reinterpret_cast<T *>(gc.allocate(sizeof(T)));
			new (ptr) T();
			gc.commit_object(*ptr);
			return *ptr;
		}

		template <class T, class A0>
		object &construct_object(garbage_collector &gc, A0 &&a0)
		{
			T * const ptr = reinterpret_cast<T *>(gc.allocate(sizeof(T)));
			new (ptr) T(std::forward<A0>(a0));
			gc.commit_object(*ptr);
			return *ptr;
		}
	}
}


#endif

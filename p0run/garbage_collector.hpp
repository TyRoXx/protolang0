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
			virtual void unmark() = 0;
			virtual void sweep(sweep_mode::type mode) = 0;
		};


		template <class T>
		object &construct_object(garbage_collector &gc)
		{
			T * const ptr = reinterpret_cast<T *>(gc.allocate(sizeof(T)));
			new (ptr) T();
			return *ptr;
		}

		template <class T, class A0>
		object &construct_object(garbage_collector &gc, A0 &&a0)
		{
			T * const ptr = reinterpret_cast<T *>(gc.allocate(sizeof(T)));
			new (ptr) T(std::forward<A0>(a0));
			return *ptr;
		}
	}
}


#endif

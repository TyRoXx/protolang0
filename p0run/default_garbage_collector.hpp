#pragma once
#ifndef P0RUN_DEFAULT_GARBAGE_COLLECTOR_HPP
#define P0RUN_DEFAULT_GARBAGE_COLLECTOR_HPP


#include "garbage_collector.hpp"
#include "p0common/final.hpp"
#include <memory>
#include <vector>


namespace p0
{
	namespace run
	{
		struct default_garbage_collector PROTOLANG0_FINAL_CLASS
				: garbage_collector
		{
			default_garbage_collector();
			~default_garbage_collector();
			virtual char *allocate(std::size_t byte_size) PROTOLANG0_FINAL_METHOD;
			virtual void deallocate(char *storage) PROTOLANG0_FINAL_METHOD;
			virtual void commit_object(object &constructed) PROTOLANG0_FINAL_METHOD;
			virtual void unmark() PROTOLANG0_FINAL_METHOD;
			virtual void sweep(sweep_mode::type mode) PROTOLANG0_FINAL_METHOD;

		private:

			struct object_handle;

			std::vector<std::unique_ptr<char []>> m_storage;
			std::vector<object_handle> m_objects;


			std::unique_ptr<char[]> remove_storage(char *storage) PROTOLANG0_NOEXCEPT;
		};
	}
}


#endif

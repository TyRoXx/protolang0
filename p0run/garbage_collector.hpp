#pragma once
#ifndef P0RUN_GARBAGE_COLLECTOR_HPP
#define P0RUN_GARBAGE_COLLECTOR_HPP


#include "object.hpp"
#include <memory>
#include <vector>


namespace p0
{
	namespace run
	{
		struct garbage_collector PROTOLANG0_FINAL_CLASS
		{
			void add_object(std::unique_ptr<object> object);
			void unmark();
			void sweep();

		private:

			std::vector<std::unique_ptr<object>> m_objects;
		};
	}
}


#endif

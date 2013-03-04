#include "garbage_collector.hpp"


namespace p0
{
	namespace run
	{
		out_of_memory::out_of_memory()
			: std::runtime_error("Out of memory")
		{
		}


		garbage_collector::~garbage_collector()
		{
		}
	}
}

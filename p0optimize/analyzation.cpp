#include "analyzation.hpp"
#include <boost/foreach.hpp>
#include <cassert>

namespace p0
{
	std::unordered_set<std::size_t> find_all_jump_destinations(p0::intermediate::function::instruction_vector const &code)
	{
		std::unordered_set<std::size_t> destinations;
		BOOST_FOREACH (p0::intermediate::instruction const &instruction, code)
		{
			switch (instruction.type())
			{
			case p0::intermediate::instruction_type::jump:
			case p0::intermediate::instruction_type::jump_if:
			case p0::intermediate::instruction_type::jump_if_not:
				destinations.insert(instruction.arguments()[0]);
				break;

			default:
				assert(!p0::intermediate::is_any_jump(instruction.type()));
				break;
			}
		}
		return destinations;
	}
}

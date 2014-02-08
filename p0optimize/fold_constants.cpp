#include "fold_constants.hpp"
#include "analyzation.hpp"
#include "local_constant_tracker.hpp"
#include <p0i/emitter.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <boost/foreach.hpp>

namespace p0
{
	p0::intermediate::function::instruction_vector fold_constants(p0::intermediate::function::instruction_vector const &original)
	{
		p0::intermediate::function::instruction_vector optimized;
		local_constant_tracker known_locals;
		auto const jump_destinations = find_all_jump_destinations(original);
		for (std::size_t i = 0; i < original.size(); ++i)
		{
			if (jump_destinations.find(i) != jump_destinations.end())
			{
				known_locals.invalidate();
			}

			auto const &original_instruction = original[i];
			track_instruction(known_locals, original_instruction);
			optimized.push_back(original_instruction);

			switch (original_instruction.type())
			{
			case p0::intermediate::instruction_type::copy:
			case p0::intermediate::instruction_type::add:
				{
					auto const destination = static_cast<local_address>(original_instruction.arguments()[0]);
					auto const known_destination = known_locals.find_current_value(destination);
					if (known_destination)
					{
						optimized.pop_back();
						p0::intermediate::emitter emitter(optimized);
						emitter.set_constant(destination, *known_destination);
					}
					break;
				}

			default:
				break;
			}
		}

		return optimized;
	}
}

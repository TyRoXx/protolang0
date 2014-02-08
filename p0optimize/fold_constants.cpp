#include "fold_constants.hpp"
#include "analyzation.hpp"
#include <p0i/emitter.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <boost/foreach.hpp>

namespace p0
{
	p0::intermediate::function::instruction_vector fold_constants(p0::intermediate::function::instruction_vector const &original)
	{
		p0::intermediate::function::instruction_vector optimized;
		boost::unordered_map<p0::local_address, p0::integer> known_locals;
		auto const jump_destinations = find_all_jump_destinations(original);
		for (std::size_t i = 0; i < original.size(); ++i)
		{
			if (jump_destinations.find(i) != jump_destinations.end())
			{
				known_locals.clear();
			}

			p0::intermediate::instruction const &original_instruction = original[i];
			optimized.push_back(original_instruction);

			switch (original_instruction.type())
			{
			case p0::intermediate::instruction_type::set_from_constant:
				{
					known_locals[static_cast<local_address>(original_instruction.arguments()[0])] = original_instruction.arguments()[1];
					break;
				}

			case p0::intermediate::instruction_type::add:
				{
					auto const destination = static_cast<local_address>(original_instruction.arguments()[0]);
					auto const known_destination = known_locals.find(destination);

					auto const source = static_cast<local_address>(original_instruction.arguments()[1]);
					auto const known_source = known_locals.find(source);

					if (known_source != known_locals.end() &&
						known_destination != known_locals.end())
					{
						//TODO: check overflow
						auto const result = (known_source->second + known_destination->second);
						optimized.pop_back();
						p0::intermediate::emitter emitter(optimized);
						emitter.set_constant(destination, result);

						known_locals[destination] = result;
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

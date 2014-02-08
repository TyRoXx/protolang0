#include "constant_folding.hpp"
#include <p0i/emitter.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <boost/foreach.hpp>

namespace p0
{
	namespace
	{
		boost::unordered_set<std::size_t> find_all_jump_destinations(p0::intermediate::function::instruction_vector const &code)
		{
			boost::unordered_set<std::size_t> destinations;
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

	p0::intermediate::function::instruction_vector fold_constants(p0::intermediate::function const &original)
	{
		p0::intermediate::function::instruction_vector optimized;
		boost::unordered_map<p0::local_address, p0::integer> known_locals;
		auto const jump_destinations = find_all_jump_destinations(original.body());
		for (std::size_t i = 0; i < original.body().size(); ++i)
		{
			if (jump_destinations.find(i) != jump_destinations.end())
			{
				known_locals.clear();
			}

			p0::intermediate::instruction const &original_instruction = original.body()[i];
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

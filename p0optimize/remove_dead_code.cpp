#include "remove_dead_code.hpp"
#include "transformation.hpp"
#include <boost/foreach.hpp>
#include <boost/dynamic_bitset.hpp>

namespace p0
{
	p0::intermediate::function::instruction_vector remove_dead_code(p0::intermediate::function::instruction_vector const &original)
	{
		boost::unordered_set<std::size_t> obsolete_instructions;
		boost::dynamic_bitset<> reachable_instructions(original.size());
		std::size_t known_count = 0;
		bool has_changed = true;
		auto const set_reachable = [&reachable_instructions, &known_count, &obsolete_instructions, &has_changed](std::size_t position)
		{
			assert(position <= reachable_instructions.size());
			if (position == reachable_instructions.size())
			{
				//this is a jump to the end of the program, we do not have to do anything in this case
				return;
			}
			bool const was = reachable_instructions[position];
			if (was)
			{
				return;
			}
			reachable_instructions[position] = true;
			has_changed = true;
		};
		if (!reachable_instructions.empty())
		{
			set_reachable(0);
		}
		while (has_changed)
		{
			has_changed = false;
			for (std::size_t i = 0; i < original.size(); ++i)
			{
				auto const &instruction = original[i];
				if (reachable_instructions[i])
				{
					switch (instruction.type())
					{
					case p0::intermediate::instruction_type::jump:
						{
							set_reachable(instruction.arguments()[0]);
							break;
						}

					case p0::intermediate::instruction_type::jump_if_not:
					case p0::intermediate::instruction_type::jump_if:
						{
							set_reachable(instruction.arguments()[0]);
							/*fall through*/
						}

					default:
						if (i + 1 < original.size())
						{
							set_reachable(i + 1);
						}
						break;
					}
				}
			}
		}

		//TODO: optimize the removal
		for (std::size_t i = 0; i < reachable_instructions.size(); ++i)
		{
			if (!reachable_instructions[i])
			{
				obsolete_instructions.insert(i);
			}
		}
		auto living = remove_instructions_at_positions(original, obsolete_instructions);
		return living;
	}
}

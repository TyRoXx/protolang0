#include "transformation.hpp"
#include <boost/foreach.hpp>

namespace p0
{
	namespace
	{
		void fix_up_jump_destinations(
				p0::intermediate::function::instruction_vector &code,
				std::vector<std::size_t> const &original_to_optimized_position)
		{
			BOOST_FOREACH (auto &instruction, code)
			{
				switch (instruction.type())
				{
				case p0::intermediate::instruction_type::jump:
				case p0::intermediate::instruction_type::jump_if:
				case p0::intermediate::instruction_type::jump_if_not:
					{
						auto &destination = instruction.arguments()[0];
						destination = original_to_optimized_position[destination];
						break;
					}

				default:
					assert(!p0::intermediate::is_any_jump(instruction.type()));
					break;
				}
			}
		}
	}

	p0::intermediate::function::instruction_vector remove_instructions_at_positions(
			p0::intermediate::function::instruction_vector const &original,
			boost::unordered_set<std::size_t> const &positions_to_remove)
	{
		p0::intermediate::function::instruction_vector optimized;
		std::vector<std::size_t> original_to_optimized_position;

		for (std::size_t i = 0; i < original.size(); ++i)
		{
			std::size_t const current_optimized_position = optimized.size();
			original_to_optimized_position.push_back(current_optimized_position);

			if (positions_to_remove.find(i) != positions_to_remove.end())
			{
				continue;
			}

			auto const &instruction = original[i];
			optimized.push_back(instruction);
		}

		original_to_optimized_position.push_back(optimized.size());

		fix_up_jump_destinations(optimized, original_to_optimized_position);
		return optimized;
	}
}

#include "remove_no_ops.hpp"
#include "analyzation.hpp"
#include <boost/foreach.hpp>
#include <boost/unordered_map.hpp>

namespace p0
{
	namespace
	{
		boost::unordered_set<std::size_t> find_obsolete_instructions(p0::intermediate::function::instruction_vector const &original)
		{
			auto const jump_destinations = find_all_jump_destinations(original);
			boost::unordered_set<std::size_t> obsolete_instructions;
			boost::unordered_map<local_address, std::size_t> unread_locals;
			for (std::size_t i = 0; i < original.size(); ++i)
			{
				if (jump_destinations.find(i) != jump_destinations.end())
				{
					unread_locals.clear();
				}

				auto const read_local = [&unread_locals](local_address which)
				{
					unread_locals.erase(which);
				};

				auto const write_local = [&unread_locals, &obsolete_instructions, i](local_address which)
				{
					auto const was_unread = unread_locals.find(which);
					if (was_unread != unread_locals.end())
					{
						obsolete_instructions.insert(was_unread->second);
					}
					unread_locals[which] = i;
				};

				auto const &instruction = original[i];
				switch (instruction.type())
				{
				case p0::intermediate::instruction_type::nothing:
					obsolete_instructions.insert(i);
					break;

				case p0::intermediate::instruction_type::set_from_constant:
					write_local(instruction.arguments()[0]);
					break;

				case p0::intermediate::instruction_type::add:
					write_local(instruction.arguments()[0]);
					read_local(instruction.arguments()[1]);
					break;

					//TODO
				}
			}

			BOOST_FOREACH (auto const &unread_local, unread_locals)
			{
				//do not optimize away the return value which is always at address 0
				local_address const result_address = 0;
				if (unread_local.first == result_address)
				{
					continue;
				}
				obsolete_instructions.insert(unread_local.second);
			}

			return obsolete_instructions;
		}

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

	p0::intermediate::function::instruction_vector remove_no_ops(p0::intermediate::function::instruction_vector const &original)
	{
		auto const obsolete_instructions = find_obsolete_instructions(original);
		auto optimized = remove_instructions_at_positions(original, obsolete_instructions);
		return optimized;
	}
}

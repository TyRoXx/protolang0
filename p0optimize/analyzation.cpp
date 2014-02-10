#include "analyzation.hpp"
#include <boost/foreach.hpp>
#include <cassert>
#include <unordered_map>
#include <set>

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

	namespace
	{
		struct section_end
		{
			p0::intermediate::instruction const *instruction_after;
			std::unordered_set<std::size_t> destinations;
		};

		template <class InstructionPtrPredicate>
		section_end find_section_end(const_instruction_range code, InstructionPtrPredicate const &is_jump_destination, std::size_t code_offset)
		{
			std::unordered_set<std::size_t> destinations;
			auto i = code.begin();
			for (;;)
			{
				if (i == code.end())
				{
					destinations.insert(code_offset + code.size());
					break;
				}

				if ((i != code.begin()) && is_jump_destination(i))
				{
					break;
				}

				if (p0::intermediate::is_any_jump(i->type()))
				{
					auto const destination = i->arguments()[0];
					destinations.insert(destination);
					if (i->type() != p0::intermediate::instruction_type::jump)
					{
						//jump_if, jump_if_not
						destinations.emplace(code_offset + std::distance(code.begin(), i) + 1);
					}
					++i;
					break;
				}

				++i;
			}
			return section_end{i, std::move(destinations)};
		}
	}

	function_section_graph dissect_function(p0::intermediate::function::instruction_vector const &code)
	{
		auto const jump_destinations = find_all_jump_destinations(code);
		std::vector<linear_section> sections;
		const_instruction_range rest{code.data(), code.data() + code.size()};
		std::unordered_map<std::size_t, section_id> instruction_to_section;
		while (!rest.empty())
		{
			auto const is_jump_destination = [&jump_destinations, &code](p0::intermediate::instruction const *i)
			{
				return (jump_destinations.find(std::distance(code.data(), i)) != jump_destinations.end());
			};
			auto const code_offset = std::distance(code.data(), rest.begin());
			auto const section_end = find_section_end(
						rest,
						is_jump_destination,
						code_offset);
			for (auto i = code_offset, e = code_offset + std::distance(rest.begin(), section_end.instruction_after); i != e; ++i)
			{
				instruction_to_section.insert(std::make_pair(i, sections.size()));
			}
			sections.emplace_back(linear_section{{rest.begin(), section_end.instruction_after}, std::move(section_end.destinations)});
			rest = {section_end.instruction_after, rest.end()};
		}
		instruction_to_section.insert(std::make_pair(code.size(), sections.size()));
		for (std::size_t i = 0; i < sections.size(); ++i)
		{
			auto &section = sections[i];
			std::unordered_set<std::size_t> real_destinations;
			std::for_each(begin(section.destinations),
						  end(section.destinations),
						  [&instruction_to_section, i, &sections, &real_destinations](std::size_t destination)
			{
				auto const real_destination = instruction_to_section[destination];
				real_destinations.emplace(real_destination);
				if (real_destination < sections.size())
				{
					sections[real_destination].origins.emplace(i);
				}
			});
			section.destinations = std::move(real_destinations);
		}
		if (!sections.empty())
		{
			sections.front().origins.emplace(calling_section);
			sections.back().destinations.emplace(sections.size());
		}
		return function_section_graph{std::move(sections)};
	}
}

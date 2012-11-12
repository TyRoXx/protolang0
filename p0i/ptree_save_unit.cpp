#include "ptree_save_unit.hpp"
#include "unit.hpp"
#include "instruction.hpp"
#include <boost/format.hpp>


namespace p0
{
	namespace intermediate
	{
		namespace
		{
			void save_instruction(
				boost::property_tree::ptree &tree,
				instruction const &instr
				)
			{
				auto const &instr_info = get_instruction_info(instr.type());

				tree.add("type", instr_info.name);

				for (size_t i = 0, c = instr_info.argument_count; i < c; ++i)
				{
					tree.add(
						(boost::format("arg%1%") % i).str(),
						instr.arguments()[i]
					);
				}
			}

			void save_function(
				boost::property_tree::ptree &tree,
				function const &function
				)
			{
				tree.add("parameters", function.parameters());

				auto const body_size = function.body().size();

				if (body_size > 0)
				{
					boost::property_tree::ptree body_tree;

					for (size_t i = 0; i < body_size; ++i)
					{
						boost::property_tree::ptree instr_tree;

						save_instruction(
							instr_tree,
							function.body()[i]
							);

						body_tree.push_back(std::make_pair("", instr_tree));
					}

					tree.add_child("body", body_tree);
				}
			}
		}


		void save_unit(
			boost::property_tree::ptree &tree,
			unit const &unit
			)
		{
			tree.add("integer-width", unit.integer_width());

			auto const function_count = unit.functions().size();
			if (function_count > 0)
			{
				boost::property_tree::ptree functions;

				for (size_t i = 0; i < function_count; ++i)
				{
					auto const &function = unit.functions()[i];

					boost::property_tree::ptree function_tree;

					save_function(
						function_tree,
						function
						);

					functions.push_back(std::make_pair("", function_tree));
				}

				tree.add_child("functions", functions);
			}
		}
	}
}

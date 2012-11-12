#include "ptree_load_unit.hpp"
#include "unit.hpp"
#include "instruction.hpp"
#include <boost/format.hpp>


namespace p0
{
	namespace intermediate
	{
		namespace
		{
			instruction load_instruction(
				boost::property_tree::ptree const &tree
				)
			{
				auto const type_name = tree.get<std::string>("type");
				auto const type = find_instruction_by_name(type_name);
				if (type == instruction_type::invalid_)
				{
					throw std::runtime_error("Invalid instruction name: " + type_name);
				}

				instruction::argument_array arguments;

				for (size_t i = 0, c = get_instruction_info(type).argument_count;
					i < c; ++i)
				{
					arguments[i] = tree.get<instruction_argument>(
						(boost::format("arg%1%") % i).str()
						);
				}

				return instruction(type, arguments);
			}

			function load_function(
				boost::property_tree::ptree const &tree
				)
			{
				auto const parameter_count = tree.get<size_t>("parameters");

				function::instruction_vector body;
				{
					auto const opt_body_tree = tree.get_child_optional("body");
					if (opt_body_tree)
					{
						auto const &body_tree = *opt_body_tree;

						for (auto i = body_tree.begin(), e = body_tree.end();
							i != e; ++i)
						{
							body.push_back(
								load_instruction(i->second)
								);
						}
					}
				}

				return function(
					std::move(body),
					parameter_count
					);
			}
		}


		unit load_unit(
			boost::property_tree::ptree const &tree
			)
		{
			auto const integer_width = tree.get<size_t>("integer-width");

			unit::function_vector functions;
			{
				auto const opt_functions_tree = tree.get_child_optional("functions");
				if (opt_functions_tree)
				{
					auto const &functions_tree = *opt_functions_tree;
					
					for (auto f = functions_tree.begin(), e = functions_tree.end();
						f != e; ++f)
					{
						functions.push_back(
							load_function(f->second)
							);
					}
				}
			}

			unit::string_vector strings;

			return unit(
				std::move(functions),
				std::move(strings),
				integer_width
				);
		}
	}
}

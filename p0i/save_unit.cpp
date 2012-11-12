#include "save_unit.hpp"
#include "unit.hpp"
#include "function.hpp"
#include <algorithm>
#include <iomanip>
#include <cassert>
#include <cmath>


namespace p0
{
	namespace intermediate
	{
		namespace
		{
			namespace
			{
				size_t get_base10_index_width(size_t count)
				{
					return count ? static_cast<size_t>(1 + std::log(static_cast<double>(count)) / std::log(10.0)) : 0;
				}
			}

			void save_instruction(
				std::ostream &file,
				instruction const &instruction,
				size_t address,
				size_t address_width
				)
			{
				using namespace std;

				auto const &info = get_instruction_info(instruction.type());

				file << std::setw(address_width) << std::setfill('0') << address << std::setfill(' ');
				file << ": ";
				file << std::setw(18) << std::left << info.name;

				std::for_each(
					begin(instruction.arguments()),
					begin(instruction.arguments()) + info.argument_count,
					[&file](instruction_argument arg)
				{
					file << ' ' << std::setw(9) << std::right << arg;
				});

				file << '\n';
			}

			void save_function(
				std::ostream &file,
				function const &function,
				size_t index
				)
			{
				file << "function " << index << "\n";
				auto const &body = function.body();
				auto const body_size = body.size();
				auto const base10_address_width = get_base10_index_width(body_size);

				for (size_t i = 0; i < body.size(); ++i)
				{
					save_instruction(file, body[i], i, base10_address_width);
				}
				file << "end\n";
			}

			void encode_string(
				std::ostream &file,
				std::string const &str
				)
			{
				for (auto i = str.begin(); i != str.end(); ++i)
				{
					if ((*i < 0x20) || (*i > 0x7e))
					{
						file << "\\x" << std::setw(2) << std::setfill('0') << std::hex
							<< (static_cast<unsigned>(*i) & 0xff);
					}
					else
					{
						switch (*i)
						{
						case '\\':
						case '\'':
						case '\"':
							file << '\\' << *i;
							break;

						default:
							file << *i;
							break;
						}
					}
				}
			}
		}


		void save_unit(
			std::ostream &file,
			unit const &unit
			)
		{
			file << "integer-width: " << unit.integer_width() << '\n';
			file << '\n';

			auto const &functions = unit.functions();
			for (size_t i = 0; i < functions.size(); ++i)
			{
				save_function(file, functions[i], i);

				file << '\n';
			}

			auto const string_count = unit.strings().size();
			auto const base10_index_width = get_base10_index_width(string_count);

			for (size_t s = 0; s < string_count; ++s)
			{
				file
					<< std::setw(base10_index_width)
					<< std::setfill('0')
					<< std::right
					<< std::dec
					<< s << ": "
				;

				encode_string(file, unit.strings()[s]);

				file
					<< '\n';
			}
		}
	}
}

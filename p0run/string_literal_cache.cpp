#include "string_literal_cache.hpp"
#include "string.hpp"
#include "p0i/unit.hpp"


namespace p0
{
	namespace run
	{
		string_literal_cache::string_literal_cache()
		{
		}

		string_literal_cache::~string_literal_cache()
		{
		}

		value string_literal_cache::require_string(
				intermediate::unit const &program,
				std::size_t string_index)
		{
			auto &program_strings = m_strings[&program];
			if (program_strings.empty())
			{
				program_strings.resize(program.strings().size());
			}
			assert(string_index < program_strings.size());
			auto &string_ptr = program_strings[string_index];
			if (!string_ptr)
			{
				string_ptr.reset(new string(program.strings()[string_index]));
			}
			return value(*string_ptr);
		}
	}
}

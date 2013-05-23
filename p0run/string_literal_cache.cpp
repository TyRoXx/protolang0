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
			assert(string_index < program.strings().size());
			auto &string_ptr = m_strings[std::make_pair(&program, string_index)];
			if (!string_ptr)
			{
				string_ptr.reset(new string(program.strings()[string_index]));
			}
			return value(*string_ptr);
		}
	}
}

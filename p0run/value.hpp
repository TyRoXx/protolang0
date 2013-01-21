#pragma once
#ifndef P0RUN_VALUE_HPP
#define P0RUN_VALUE_HPP


#include <cstdint>


namespace p0
{
	namespace run
	{
		typedef std::int64_t integer;


		namespace value_type
		{
			enum Enum
			{
				integer,
				null,
			};
		}


		struct value
		{
			value_type::Enum type;
			union
			{
				integer i;
			};


			value();
			explicit value(integer i);
		};
	}
}


#endif

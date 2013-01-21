#pragma once
#ifndef P0RUN_VALUE_HPP
#define P0RUN_VALUE_HPP


#include <cstdint>


namespace p0
{
	namespace intermediate
	{
		struct function;
	}


	namespace run
	{
		typedef std::int64_t integer;


		namespace value_type
		{
			enum Enum
			{
				integer,
				null,
				function_ptr,

				count_,
			};
		}


		struct value
		{
			value_type::Enum type;
			union
			{
				integer i;
				intermediate::function const *function_ptr;
			};


			value();
			explicit value(integer i);
			explicit value(intermediate::function const &function_ptr);
		};


		bool to_boolean(value const &value);


		namespace comparison_result
		{
			enum Enum
			{
				equal,
				less,
				greater,
			};
		}
		comparison_result::Enum compare(value const &left, value const &right);
	}
}


#endif

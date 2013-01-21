#pragma once
#ifndef P0RUN_VALUE_HPP
#define P0RUN_VALUE_HPP


#include <cassert>
#include <cstdint>
#include <functional>


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


		bool operator == (value const &left, value const &right);
		bool operator < (value const &left, value const &right);
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

namespace std
{
	template <>
	struct hash<p0::run::value>
	{
	public:

		inline std::size_t operator()(const p0::run::value &value) const
		{
			using namespace p0::run::value_type;
			switch (value.type)
			{
			case null:
				return -1;

			case integer:
				return std::hash<p0::run::integer>()(value.i);

			case function_ptr:
				return std::hash<p0::intermediate::function const *>()(value.function_ptr);

			default:
				assert(!"Invalid type");
				return 0;
			}
		}
	};
}


#endif

#pragma once
#ifndef P0RUN_VALUE_HPP
#define P0RUN_VALUE_HPP


#include "p0common/final.hpp"
#include <cassert>
#include <cstdint>
#include <functional>
#include <ostream>


namespace p0
{
	namespace intermediate
	{
		struct function;
	}


	namespace run
	{
		typedef std::int64_t integer;
		struct object;


		namespace value_type
		{
			enum Enum
			{
				integer,
				null,
				function_ptr,
				object
			};

			enum
			{
				count_ = 4
			};
		}


		struct value PROTOLANG0_FINAL_CLASS
		{
			value_type::Enum type;
			union
			{
				integer i;
				intermediate::function const *function_ptr;
				object *obj;
			};


			value();
			explicit value(integer i);
			explicit value(intermediate::function const &function_ptr);
			explicit value(object &obj);
		};


		bool operator == (value const &left, value const &right);
		bool operator < (value const &left, value const &right);
		bool to_boolean(value const &value);
		bool is_null(value const &value);


		namespace comparison_result
		{
			enum Enum
			{
				equal,
				less,
				greater,
			};
		}

		template <class T>
		comparison_result::Enum compare(T left, T right)
		{
			auto const diff = (left - right);
			if (diff < 0)
			{
				return comparison_result::less;
			}
			else if (diff > 0)
			{
				return comparison_result::greater;
			}
			return comparison_result::equal;
		}

		comparison_result::Enum compare(value const &left, value const &right);

		std::ostream &operator << (std::ostream &out, value const &value);
	}
}

namespace std
{
	template <>
	struct hash<p0::run::value>
	{
	public:

		std::size_t operator()(const p0::run::value &value) const;
	};
}


#endif

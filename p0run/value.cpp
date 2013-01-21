#include "value.hpp"
#include <cassert>
#include <boost/static_assert.hpp>


namespace p0
{
	namespace run
	{
		value::value()
			: type(value_type::null)
		{
		}

		value::value(integer i)
			: type(value_type::integer)
		{
			this->i = i;
		}

		value::value(intermediate::function const &function_ptr)
			: type(value_type::function_ptr)
		{
			this->function_ptr = &function_ptr;
		}


		bool to_boolean(value const &value)
		{
			BOOST_STATIC_ASSERT(value_type::count_ == 3);
			switch (value.type)
			{
			case value_type::integer:
				return (value.i != 0);

			case value_type::null:
				return false;

			case value_type::function_ptr:
				return true;

			default:
				assert(!"Invalid value type");
				return false;
			}
		}

		namespace
		{
			integer to_comparable_integer(value const &value)
			{
				switch (value.type)
				{
				case value_type::integer:
					return value.i;

				case value_type::null:
					return 0;

				case value_type::function_ptr:
					return 1;

				default:
					return 0;
				}
			}
		}

		comparison_result::Enum compare(value const &left, value const &right)
		{
			auto const diff = (to_comparable_integer(left) - to_comparable_integer(right));
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
	}
}

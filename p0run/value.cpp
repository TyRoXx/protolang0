#include "value.hpp"
#include "p0i/function.hpp"
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


		bool operator == (value const &left, value const &right)
		{
			return compare(left, right) == comparison_result::equal;
		}

		bool operator < (value const &left, value const &right)
		{
			return compare(left, right) == comparison_result::less;
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
			comparison_result::Enum compare_impl(
					integer left, value const &right)
			{
				switch (right.type)
				{
				case value_type::integer:
					return compare(left, right.i);

				case value_type::null:
					return comparison_result::greater;

				case value_type::function_ptr:
					return comparison_result::greater;

				default:
					assert(!"Invalid value type");
					return comparison_result::equal;
				}
			}

			comparison_result::Enum compare_impl(
					intermediate::function const &left, value const &right)
			{
				switch (right.type)
				{
				case value_type::integer:
					return comparison_result::less;

				case value_type::null:
					return comparison_result::greater;

				case value_type::function_ptr:
					return compare(&left, right.function_ptr);

				default:
					assert(!"Invalid value type");
					return comparison_result::equal;
				}
			}
		}

		comparison_result::Enum compare(value const &left, value const &right)
		{
			switch (left.type)
			{
			case value_type::integer:
				return compare_impl(left.i, right);

			case value_type::null:
				switch (right.type)
				{
				case value_type::integer:
					return comparison_result::less;

				case value_type::null:
					return comparison_result::equal;

				case value_type::function_ptr:
					return comparison_result::less;
				}

			case value_type::function_ptr:
				return compare_impl(*left.function_ptr, right);

			default:
				assert(!"Invalid value type");
				return comparison_result::equal;
			}
		}
	}
}
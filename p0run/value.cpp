#include "value.hpp"
#include "object.hpp"
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

		value::value(object &obj)
			: type(value_type::object)
		{
			this->obj = &obj;
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
			BOOST_STATIC_ASSERT(value_type::count_ == 4);
			switch (value.type)
			{
			case value_type::integer:
				return (value.i != 0);

			case value_type::null:
				return false;

			case value_type::function_ptr:
				return true;

			case value_type::object:
				return true;
			}
			assert(!"Invalid value type");
			return false;
		}

		bool is_null(value const &value)
		{
			return (value.type == value_type::null);
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

				case value_type::object:
					return comparison_result::greater;
				}
				assert(!"Invalid value type");
				return comparison_result::equal;
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

				case value_type::object:
					return comparison_result::less;
				}
				assert(!"Invalid value type");
				return comparison_result::equal;
			}

			comparison_result::Enum compare_impl(
					object const &left, value const &right)
			{
				switch (right.type)
				{
				case value_type::integer:
					return comparison_result::less;

				case value_type::null:
					return comparison_result::greater;

				case value_type::function_ptr:
					return comparison_result::greater;

				case value_type::object:
					return compare<object const *>(&left, right.obj);
				}
				assert(!"Invalid value type");
				return comparison_result::equal;
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

				case value_type::object:
					return comparison_result::less;
				}

			case value_type::function_ptr:
				return compare_impl(*left.function_ptr, right);

			case value_type::object:
				return compare_impl(*left.obj, right);
			}

			assert(!"Invalid value type");
			return comparison_result::equal;
		}
	}
}

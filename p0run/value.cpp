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
	}
}

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
			assert(nullptr == "Invalid value type");
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
				assert(nullptr == "Invalid value type");
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
				assert(nullptr == "Invalid value type");
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
					assert(right.obj);
					return left.compare(*right.obj);
				}
				assert(nullptr == "Invalid value type");
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

			assert(nullptr == "Invalid value type");
			return comparison_result::equal;
		}

		std::ostream &operator << (std::ostream &out, value const &value)
		{
			switch (value.type)
			{
			case value_type::null:
				out << "null";
				break;

			case value_type::integer:
				out << value.i;
				break;

			case value_type::function_ptr:
				out << "function";
				break;

			case value_type::object:
				value.obj->print(out);
				break;
			}
			return out;
		}
	}
}

std::size_t std::hash<p0::run::value>::operator()(const p0::run::value &value) const
{
	using namespace p0::run::value_type;
	switch (value.type)
	{
	case null:
		return static_cast<size_t>(-1);

	case integer:
		return std::hash<p0::run::integer>()(value.i);

	case function_ptr:
		return std::hash<p0::intermediate::function const *>()
			(value.function_ptr);

	case object:
		return static_cast<std::size_t>(value.obj->get_hash_code());

	default:
		assert(nullptr == "Invalid type");
		return 0;
	}
}

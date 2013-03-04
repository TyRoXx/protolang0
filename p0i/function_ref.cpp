#include "function_ref.hpp"
#include <cassert>
#include <boost/functional/hash.hpp>


namespace p0
{
	namespace intermediate
	{
		function_ref::function_ref()
			: m_origin(nullptr)
			, m_function(nullptr)
		{
		}

		function_ref::function_ref(unit const &origin, intermediate::function const &function)
			: m_origin(&origin)
			, m_function(&function)
		{
		}

		unit const &function_ref::origin() const
		{
			assert(is_set());
			assert(m_origin);
			return *m_origin;
		}

		function const &function_ref::function() const
		{
			assert(is_set());
			assert(m_function);
			return *m_function;
		}

		bool function_ref::is_set() const
		{
			return (m_origin != nullptr);
		}

		unit const *function_ref::origin_ptr() const
		{
			return m_origin;
		}

		intermediate::function const *function_ref::function_ptr() const
		{
			return m_function;
		}


		std::pair<unit const *, function const *> to_pair(function_ref const &ref)
		{
			return std::make_pair(ref.origin_ptr(), ref.function_ptr());
		}

		bool operator == (function_ref const &left, function_ref const &right)
		{
			return to_pair(left) == to_pair(right);
		}

		bool operator < (function_ref const &left, function_ref const &right)
		{
			//make std::pair do the work
			return to_pair(left) < to_pair(right);
		}
	}
}

namespace std
{
	std::size_t hash<p0::intermediate::function_ref>::operator()(const p0::intermediate::function_ref &ref) const
	{
		auto const pair = to_pair(ref);
		//unfortunately, std::hash is not specialized for std::pair,
		//so we use boost here
		return boost::hash_value(pair);
	}
}

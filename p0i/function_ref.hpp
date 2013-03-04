#pragma once
#ifndef P0I_FUNCTION_REF_HPP
#define P0I_FUNCTION_REF_HPP


#include "p0common/final.hpp"
#include <utility>
#include <functional>


namespace p0
{
	namespace intermediate
	{
		struct function;
		struct unit;


		struct function_ref PROTOLANG0_FINAL_CLASS
		{
			function_ref();
			function_ref(unit const &origin, intermediate::function const &function);
			unit const &origin() const;
			intermediate::function const &function() const;
			bool is_set() const;
			unit const *origin_ptr() const;
			intermediate::function const *function_ptr() const;

		private:

			unit const *m_origin;
			intermediate::function const *m_function;
		};


		std::pair<unit const *, function const *> to_pair(function_ref const &ref);


		bool operator == (function_ref const &left, function_ref const &right);
		bool operator < (function_ref const &left, function_ref const &right);
	}
}

namespace std
{
	template <>
	struct hash<p0::intermediate::function_ref>
	{
	public:

		std::size_t operator()(const p0::intermediate::function_ref &ref) const;
	};
}


#endif

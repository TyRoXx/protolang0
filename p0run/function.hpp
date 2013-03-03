#pragma once
#ifndef P0RUN_FUNCTION_HPP
#define P0RUN_FUNCTION_HPP


#include "value.hpp"
#include "object.hpp"
#include "p0common/final.hpp"


namespace p0
{
	namespace run
	{
		struct function PROTOLANG0_FINAL_CLASS : object
		{
			explicit function(intermediate::function const &function);
			virtual boost::optional<value> call(
				std::vector<value> const &arguments,
				interpreter &interpreter
				) PROTOLANG0_FINAL_METHOD;
			virtual void print(std::ostream &out) const PROTOLANG0_FINAL_METHOD;
			virtual bool bind(size_t index, value const &value) PROTOLANG0_FINAL_METHOD;
			virtual boost::optional<value> get_bound(size_t index) const PROTOLANG0_FINAL_METHOD;

		private:

			intermediate::function const &m_function;
			std::vector<value> m_bound_variables;


			virtual void mark_recursively() PROTOLANG0_FINAL_METHOD;
		};
	}
}


#endif

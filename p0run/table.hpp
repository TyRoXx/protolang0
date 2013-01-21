#pragma once
#ifndef P0RUN_TABLE_HPP
#define P0RUN_TABLE_HPP


#include "value.hpp"
#include <unordered_map>

namespace p0
{
	namespace run
	{
		struct table
		{
			void set_element(value const &key, value const &value);
			value get_element(value const &key) const;

		private:

			typedef std::unordered_map<value, value> elements;


			elements m_elements;
		};
	}
}


#endif

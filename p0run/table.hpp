#pragma once
#ifndef P0RUN_TABLE_HPP
#define P0RUN_TABLE_HPP


#include "value.hpp"
#include "object.hpp"
#include "p0common/final.hpp"
#include <unordered_map>


namespace p0
{
	namespace run
	{
		struct table PROTOLANG0_FINAL_CLASS : object
		{
			typedef std::unordered_map<value, value> elements;


			table();
			explicit table(elements elements);
			virtual boost::optional<value> get_element(value const &key) const PROTOLANG0_FINAL_METHOD;
			virtual bool set_element(value const &key, value const &value) PROTOLANG0_FINAL_METHOD;

		private:

			elements m_elements;


			virtual void mark_recursively() PROTOLANG0_FINAL_METHOD;
		};
	}
}


#endif

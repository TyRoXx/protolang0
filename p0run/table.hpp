#pragma once
#ifndef P0RUN_TABLE_HPP
#define P0RUN_TABLE_HPP


#include "value.hpp"
#include "object.hpp"
#include <unordered_map>


namespace p0
{
	namespace run
	{
		struct table : object
		{
			virtual void mark_recursively() override;
			virtual boost::optional<value> get_element(value const &key) const override;
			virtual bool set_element(value const &key, value const &value) override;

		private:

			typedef std::unordered_map<value, value> elements;


			elements m_elements;
		};
	}
}


#endif

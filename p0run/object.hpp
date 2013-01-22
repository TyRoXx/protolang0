#pragma once
#ifndef P0RUN_OBJECT_HPP
#define P0RUN_OBJECT_HPP


#include "value.hpp"
#include <boost/optional.hpp>


namespace p0
{
	namespace run
	{
		struct object
		{
			object();
			virtual ~object();
			void unmark();
			bool is_marked() const;
			virtual void mark_recursively() = 0;
			virtual boost::optional<value> get_element(value const &key) const = 0;
			virtual bool set_element(value const &key, value const &value) = 0;

		private:

			bool m_is_marked;
		};
	}
}


#endif

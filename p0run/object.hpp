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
			void mark();
			void unmark();
			bool is_marked() const;
			virtual boost::optional<value> get_element(value const &key) const = 0;
			virtual bool set_element(value const &key, value const &value) = 0;
			virtual integer get_hash_code() const;
			virtual bool equals(object const &other) const;

		private:

			bool m_is_marked;


			virtual void mark_recursively() = 0;
		};
	}
}


#endif

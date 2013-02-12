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
			virtual boost::optional<value> get_element(value const &key) const;
			virtual bool set_element(value const &key, value const &value);
			virtual integer get_hash_code() const;
			virtual bool equals(object const &other) const;
			virtual comparison_result::Enum compare(object const &right) const;
			virtual boost::optional<value> call(std::vector<value> const &arguments) const;
			virtual void print(std::ostream &out) const;

		private:

			bool m_is_marked;


			virtual void mark_recursively() = 0;
		};
	}
}


#endif

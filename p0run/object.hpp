#pragma once
#ifndef P0RUN_OBJECT_HPP
#define P0RUN_OBJECT_HPP


#include "value.hpp"
#include <boost/optional.hpp>
#include <vector>
#include <memory>


namespace p0
{
	namespace run
	{
		struct interpreter;


		struct object_element_callback
		{
			virtual ~object_element_callback();
			virtual bool handle_element(value key, value value) = 0;
		};


		struct object
		{
			object();
			virtual ~object();
			void mark();
			void unmark();
			bool is_marked() const;
			virtual boost::optional<value> get_element(value const &key) const;
			virtual bool set_element(value const &key, value const &value);
			virtual void enumerate_elements(object_element_callback &handler) const;
			virtual integer get_hash_code() const;
			virtual bool equals(object const &other) const;
			virtual comparison_result::Enum compare(object const &right) const;
			virtual boost::optional<value> call(
				std::vector<value> const &arguments,
				interpreter &interpreter
				);
			virtual boost::optional<value> call_method(
					value const &method_name,
					std::vector<value> const &arguments,
					interpreter &interpreter
					);
			virtual void print(std::ostream &out) const;
			virtual bool bind(size_t index, value const &value);
			virtual boost::optional<value> get_bound(size_t index) const;

		private:

			bool m_is_marked;


			virtual void mark_recursively() = 0;
		};


		template <class Down>
		Down *to_object(value const &value)
		{
			if (value.type == value_type::object)
			{
				return dynamic_cast<Down *>(value.obj);
			}
			return nullptr;
		}
	}
}


#endif

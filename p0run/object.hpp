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


		struct object
		{
			object();
			virtual ~object();
			void mark();
			void unmark();
			bool is_marked() const;
			void mark_permanently();
			void unmark_permanently();
			bool is_marked_permanently() const;
			virtual boost::optional<value> get_element(value const &key) const;
			virtual bool set_element(value const &key, value const &value);
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
			bool m_is_marked_permanently;


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


		struct object_permanent_unmarker PROTOLANG0_FINAL_CLASS
		{
			void operator ()(run::object *object) const
			{
				assert(object);
				object->unmark_permanently();
			}
		};

		typedef std::unique_ptr<run::object, object_permanent_unmarker>
			permanent_object_ptr;

		permanent_object_ptr make_permanent(run::object &object);
	}
}


#endif

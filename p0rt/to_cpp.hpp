#pragma once
#ifndef P0RT_TO_CPP_HPP
#define P0RT_TO_CPP_HPP


#include "p0run/string.hpp"
#include <stdexcept>
#include <map>
#include <unordered_map>
#include <boost/unordered_map.hpp>


namespace p0
{
	namespace rt
	{
		inline void to_cpp(run::value &to,
		                   run::value from)
		{
			to = from;
		}

		template <class Integer,
		          class IsCompatible = typename std::enable_if<boost::is_integral<Integer>::value, void>::type>
		void to_cpp(Integer &to,
		            run::value from)
		{
			to = static_cast<Integer>(run::to_integer(from));
		}

		inline void to_cpp(bool &to,
		                   run::value from)
		{
			to = run::to_boolean(from);
		}

		inline void to_cpp(std::string &to,
		                   run::value from)
		{
			to = run::expect_string(from);
		}

		namespace detail
		{
			template <class Key, class Value, class Map>
			void to_cpp_map(Map &to, run::value from)
			{
				if (from.type != run::value_type::object)
				{
					throw std::runtime_error("Cannot construct std::map from a non-object");
				}

				struct map_builder : run::object_element_callback
				{
					explicit map_builder(Map &to)
					    : to(to)
					{
					}

					virtual bool handle_element(run::value key,
					                            run::value value) PROTOLANG0_OVERRIDE
					{
						using p0::rt::to_cpp;

						Key cpp_key;
						to_cpp(cpp_key, key);

						Value cpp_value;
						to_cpp(cpp_value, value);

						to.insert(std::make_pair(std::move(cpp_key), std::move(cpp_value)));
						return true;
					}

				private:

					Map &to;
				};

				map_builder callback((to));
				from.obj->enumerate_elements(callback);
			}
		}

		template <class Key, class Value, class Compare, class Alloc>
		void to_cpp(std::map<Key, Value, Compare, Alloc> &to,
		            run::value from)
		{
			return detail::to_cpp_map<Key, Value>(to, from);
		}

		template <class Key, class Value, class Hash, class Pred, class Alloc>
		void to_cpp(std::unordered_map<Key, Value, Hash, Pred, Alloc> &to,
		            run::value from)
		{
			return detail::to_cpp_map<Key, Value>(to, from);
		}

		template <class Key, class Value, class Hash, class Pred, class Alloc>
		void to_cpp(boost::unordered_map<Key, Value, Hash, Pred, Alloc> &to,
		            run::value from)
		{
			return detail::to_cpp_map<Key, Value>(to, from);
		}
	}
}


#endif

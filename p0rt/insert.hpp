#pragma once
#ifndef P0RT_INSERT_HPP
#define P0RT_INSERT_HPP


#include "p0run/object.hpp"
#include "expose.hpp"


namespace p0
{
	namespace rt
	{
		void insert(run::object &table,
					run::garbage_collector &gc,
					std::string key,
					run::value const &element);

		void insert(run::object &table,
					run::garbage_collector &gc,
					std::string key,
					run::integer element);

		void insert(run::object &table,
					run::garbage_collector &gc,
					std::string key,
					std::string element);

		template <class Functor>
		void insert(run::object &table,
					run::garbage_collector &gc,
					std::string key,
					function_tag,
					Functor &&element)
		{
			return insert(table,
				   gc,
				   std::move(key),
				   expose(gc, function_tag(), std::forward<Functor>(element)));
		}


		struct inserter PROTOLANG0_FINAL_CLASS
		{
			explicit inserter(run::object &object,
							  run::garbage_collector &gc
							  );
			inserter &insert(std::string key, run::value const &element);
			inserter &insert(std::string key, run::integer element);
			inserter &insert(std::string key, std::string element);

			template <class Functor>
			inserter &insert_fn(std::string key, Functor &&element)
			{
				rt::insert(m_object, m_gc, std::move(key),
						   function_tag(), std::forward<Functor>(element));
				return *this;
			}

		private:

			run::object &m_object;
			run::garbage_collector &m_gc;
		};
	}
}


#endif

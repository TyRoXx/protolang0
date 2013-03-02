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
					run::interpreter &interpreter,
					std::string key,
					run::value const &element);

		void insert(run::object &table,
					run::interpreter &interpreter,
					std::string key,
					run::integer element);

		void insert(run::object &table,
					run::interpreter &interpreter,
					std::string key,
					std::string element);

		template <class Functor>
		void insert(run::object &table,
					run::interpreter &interpreter,
					std::string key,
					function_tag,
					Functor &&element)
		{
			return insert(table,
				   interpreter,
				   std::move(key),
				   expose(interpreter, function_tag(), std::forward<Functor>(element)));
		}


		struct inserter PROTOLANG0_FINAL_CLASS
		{
			explicit inserter(run::object &object,
							  run::interpreter &interpreter
							  );
			inserter &insert(std::string key, run::value const &element);
			inserter &insert(std::string key, run::integer element);
			inserter &insert(std::string key, std::string element);

			template <class Functor>
			inserter &insert_fn(std::string key, Functor &&element)
			{
				rt::insert(m_object, m_interpreter, std::move(key),
						   function_tag(), std::forward<Functor>(element));
				return *this;
			}

		private:

			run::object &m_object;
			run::interpreter &m_interpreter;
		};
	}
}


#endif

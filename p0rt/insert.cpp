#include "insert.hpp"


namespace p0
{
	namespace rt
	{
		void insert(run::object &table,
					run::interpreter &interpreter,
					std::string key,
					run::value const &element)
		{
			if (!table.set_element(
				run::value(expose(interpreter, std::move(key))),
				element
				))
			{
				//TODO better error handling
				throw std::runtime_error("Cannot set element in table");
			}
		}

		void insert(run::object &table,
					run::interpreter &interpreter,
					std::string key,
					std::string element)
		{
			return insert(table, interpreter, std::move(key),
						  expose(interpreter, std::move(element)));
		}

		void insert(run::object &table,
					run::interpreter &interpreter,
					std::string key,
					run::integer element)
		{
			return insert(table, interpreter, std::move(key), element);
		}
	}
}

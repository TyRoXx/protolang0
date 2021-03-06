#include "expose.hpp"


namespace p0
{
	namespace rt
	{
		run::value expose(run::garbage_collector &gc, std::string content)
		{
			return run::value(run::construct<run::string>(
								  gc, std::move(content)));
		}

		run::value expose(run::garbage_collector &gc, run::table::elements content)
		{
			return run::value(run::construct<run::table>(
								  gc, std::move(content)));
		}
	}
}

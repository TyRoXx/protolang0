#include "expose.hpp"


namespace p0
{
	namespace rt
	{
		run::value expose(run::interpreter &interpreter, std::string content)
		{
			return run::value(interpreter.register_object(
				std::unique_ptr<run::object>(new run::string(std::move(content)))));
		}

		run::value expose(run::interpreter &interpreter, run::table::elements content)
		{
			return run::value(interpreter.register_object(
				std::unique_ptr<run::object>(new run::table(std::move(content)))));
		}
	}
}

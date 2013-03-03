#include "function.hpp"
#include "interpreter.hpp"
#include "p0i/function.hpp"


namespace p0
{
	namespace run
	{
		function::function(intermediate::function const &function)
			: m_function(function)
			, m_bound_variables(function.bound_variables())
		{
		}

		boost::optional<value> function::call(
			std::vector<value> const &arguments,
			interpreter &interpreter) const
		{
			return interpreter.call(m_function, value(m_function), arguments);
		}

		void function::print(std::ostream &out) const
		{
			out << "function"; //TODO
		}

		bool function::bind(size_t index, value const &value)
		{
			if (index < m_bound_variables.size())
			{
				m_bound_variables[index] = value;
				return true;
			}
			return false;
		}

		boost::optional<value> function::get_bound(size_t index) const
		{
			if (index < m_bound_variables.size())
			{
				return m_bound_variables[index];
			}
			return boost::optional<value>();
		}


		void function::mark_recursively()
		{
			std::for_each(begin(m_bound_variables),
						  end(m_bound_variables),
						  [](value const &variable)
			{
				if (variable.type == value_type::object)
				{
					variable.obj->mark();
				}
			});
		}
	}
}

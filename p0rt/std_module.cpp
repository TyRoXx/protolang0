#include "std_module.hpp"
#include "insert.hpp"
#include "p0run/garbage_collector.hpp"
#include "p0run/interpreter.hpp"
#include "p0common/not_implemented.hpp"
#include <boost/foreach.hpp>
#include <vector>
#include <sstream>


namespace p0
{
	namespace rt
	{
		namespace
		{
			run::value std_print_string(std::vector<run::value> const &arguments)
			{
				auto &out = std::cout;
				BOOST_FOREACH (auto &argument, arguments)
				{
					out << argument;
				}
				return run::value();
			}

			run::value std_read_line(
				run::garbage_collector &gc,
				std::vector<run::value> const & /*arguments*/)
			{
				auto &in = std::cin;
				std::string line;
				getline(in, line);
				return rt::expose(gc, std::move(line));
			}

			run::value std_assert(std::vector<run::value> const &arguments)
			{
				if (arguments.empty() ||
					!run::to_boolean(arguments.front()))
				{
					//TODO better handling
					throw std::runtime_error("Assertion failed");
				}
				return run::value();
			}

			run::value std_to_string(run::garbage_collector &gc,
									 std::vector<run::value> const &arguments)
			{
				std::ostringstream result;
				BOOST_FOREACH (auto &argument, arguments)
				{
					result << argument;
				}
				return rt::expose(gc, result.str());
			}

			run::value std_class(std::vector<run::value> const &arguments)
			{
				if (arguments.empty())
				{
					return run::value();
				}

				return arguments.front();
			}

			struct class_instance PROTOLANG0_FINAL_CLASS : run::object
			{
				explicit class_instance(run::object &definition)
					: m_definition(definition)
				{
				}

				virtual boost::optional<run::value> get_element(run::value const &key) const PROTOLANG0_FINAL_METHOD
				{
					return m_fields.get_element(key);
				}

				virtual bool set_element(run::value const &key, run::value const &value) PROTOLANG0_FINAL_METHOD
				{
					return m_fields.set_element(key, value);
				}

				virtual boost::optional<run::value> call_method(
						run::value const &method_name,
						std::vector<run::value> const &arguments,
						run::interpreter &interpreter
						) PROTOLANG0_FINAL_METHOD
				{
					auto const method = m_definition.get_element(method_name);
					if (!method)
					{
						P0_NOT_IMPLEMENTED();
					}
					std::vector<run::value> arguments_with_this;
					arguments_with_this.push_back(run::value(*this));
					arguments_with_this.insert(arguments_with_this.end(),
											   arguments.begin(),
											   arguments.end());
					switch (method->type)
					{
					case run::value_type::function_ptr:
						return interpreter.call(
									*method->function_ptr,
									arguments_with_this);

					case run::value_type::object:
						return method->obj->call(arguments, interpreter);

					default:
						P0_NOT_IMPLEMENTED();
					}
				}

			private:

				run::object &m_definition;
				run::table m_fields;


				virtual void mark_recursively() PROTOLANG0_FINAL_METHOD
				{
					m_definition.mark();
					m_fields.mark();
				}
			};

			run::value std_new(run::garbage_collector &gc,
							   std::vector<run::value> const &arguments)
			{
				if (arguments.empty())
				{
					return run::value();
				}
				auto const definition = arguments.front();
				if (definition.type != run::value_type::object)
				{
					P0_NOT_IMPLEMENTED();
				}
				return run::value(run::construct_object<class_instance>(
									  gc,
									  *definition.obj));
			}
		}

		run::value register_standard_module(run::garbage_collector &gc)
		{
			auto &module = run::construct_object<run::table>(gc);
			rt::inserter(module, gc)
				.insert_fn("print", &std_print_string)
				.insert_fn("read_line", std::bind(std_read_line, std::ref(gc), std::placeholders::_1))
				.insert_fn("assert", &std_assert)
				.insert_fn("to_string", std::bind(std_to_string, std::ref(gc), std::placeholders::_1))
				.insert_fn("class", &std_class)
				.insert_fn("new", std::bind(std_new, std::ref(gc), std::placeholders::_1))
				;
			return run::value(module);
		}
	}
}

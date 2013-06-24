#include "tempest_module.hpp"
#include "p0run/garbage_collector.hpp"
#include "p0run/table.hpp"
#include "p0common/not_implemented.hpp"
#include "tempest/tcp_acceptor.hpp"
#include "p0rt/insert.hpp"
#include <boost/bind.hpp>
#include <boost/unordered_map.hpp>

namespace p0
{
	namespace tempest
	{
		std::string const default_module_name = "tempest";

		template <class Type>
		struct method_map
		{
			typedef run::value (Type::*method)(std::vector<run::value> const &);
			typedef boost::unordered_map<std::string, method> string_to_method;

			void add_method(std::string method_name,
			                method method)
			{
				m_mapping.insert(std::make_pair(std::move(method_name), method));
			}

			boost::optional<run::value> call(
			        Type &object,
			        std::string const &method_name,
			        std::vector<run::value> const &arguments) const
			{
				auto const m = m_mapping.find(method_name);
				if (m == m_mapping.end())
				{
					return boost::optional<run::value>();
				}

				auto const method = m->second;
				return (object.*method)(arguments);
			}

		private:

			string_to_method m_mapping;
		};

		struct http_server : p0::run::object
		{
			explicit http_server(boost::uint16_t port,
			                     p0::run::value on_client)
			    : m_acceptor(port, boost::bind(&http_server::on_client, this, _1), m_io_service)
			    , m_on_client(on_client)
			{
			}

			virtual boost::optional<run::value> call_method(
					run::value const &method_name,
					std::vector<run::value> const &arguments,
					run::interpreter &interpreter
					) PROTOLANG0_OVERRIDE
			{
				(void)interpreter;
				return get_methods().call(*this,
				                          run::expect_string(method_name),
				                          arguments);
			}

		private:

			boost::asio::io_service m_io_service;
			::tempest::tcp_acceptor m_acceptor;
			p0::run::value m_on_client;

			virtual void mark_recursively() PROTOLANG0_OVERRIDE
			{
				mark_value(m_on_client);
			}

			void on_client(::tempest::tcp_acceptor::client_ptr &client)
			{
				//TODO
			}

			run::value run(std::vector<run::value> const &arguments)
			{
				(void)arguments;
				m_io_service.run();
				return run::value();
			}

			run::value poll(std::vector<run::value> const &arguments)
			{
				(void)arguments;
				m_io_service.poll();
				return run::value();
			}

			static method_map<http_server> const &get_methods()
			{
				struct methods : method_map<http_server>
				{
					methods()
					{
						add_method("run", &http_server::run);
						add_method("poll", &http_server::poll);
					}
				};
				static methods const value;
				return value;
			}
		};

		run::value create_http_server(std::vector<run::value> const &arguments,
		                              run::garbage_collector &gc)
		{
			if (arguments.size() < 2)
			{
				return run::value();
			}
			auto const port = static_cast<boost::uint16_t>(run::to_integer(arguments[0]));
			auto const on_client = arguments[1];
			return run::value(run::construct<http_server>(gc, port, on_client));
		}

		run::value register_tempest_module(run::garbage_collector &gc)
		{
			auto &module = run::construct<run::table>(gc);
			rt::inserter inserter(module, gc);
			inserter.insert_fn("create_http_server",
			                   boost::bind(create_http_server, _1, boost::ref(gc)));
			return run::value(module);
		}
	}
}

#include "net_module.hpp"
#include "insert.hpp"
#include "p0common/not_implemented.hpp"
#include "p0common/unique_value.hpp"
#include "p0run/garbage_collector.hpp"
#include <boost/asio.hpp>


namespace p0
{
	namespace rt
	{
		namespace
		{
			struct ownerships
			{
				void add(run::object &object)
				{
					m_owned.push_back(&object);
				}

				void remove(run::object &object)
				{
					auto const e = std::partition(
								begin(m_owned),
								end(m_owned),
								[&object](run::object *owned)
					{
							return (&object != owned);
					});
					m_owned.erase(e, end(m_owned));
				}

				void mark()
				{
					std::for_each(begin(m_owned),
								  end(m_owned),
								  [](run::object *owned)
					{
						owned->mark();
					});
				}

			private:

				std::vector<run::object *> m_owned;
			};

			struct object_handle
			{
				explicit object_handle(
						ownerships &ownerships,
						run::object &object)
					: m_ownerships(ownerships)
					, m_object(object)
				{
					m_ownerships.add(m_object);
				}

				~object_handle()
				{
					m_ownerships.remove(m_object);
				}

			private:

				ownerships &m_ownerships;
				run::object &m_object;
			};

			std::shared_ptr<object_handle> make_object_handle(
					ownerships &ownerships,
					run::value const &value)
			{
				if (value.type == run::value_type::object)
				{
					return std::make_shared<object_handle>(
								ownerships,
								*value.obj);
				}
				return nullptr;
			}

			struct io_service PROTOLANG0_FINAL_CLASS : run::object
			{
				explicit io_service()
				{
				}

				virtual boost::optional<run::value> call_method(
						run::value const &method_name,
						std::vector<run::value> const &arguments,
						run::interpreter &interpreter
						) PROTOLANG0_FINAL_METHOD
				{
					run::string const * const method_name_str =
							run::to_object<run::string>(method_name);
					if (!method_name_str)
					{
						return boost::optional<run::value>();
					}

					if (method_name_str->content() == "post")
					{
						if (arguments.empty())
						{
							return boost::optional<run::value>();
						}
						auto const callback = arguments.front();
						auto const callback_handle = make_object_handle(
									m_callback_ownerships, callback);
						m_instance.post([this, callback, callback_handle, &interpreter]()
						{
							P0_NOT_IMPLEMENTED();
						});
					}
					else if (method_name_str->content() == "run")
					{
						m_instance.run();
					}
					return run::value();
				}

			private:

				boost::asio::io_service m_instance;
				ownerships m_callback_ownerships;


				virtual void mark_recursively() PROTOLANG0_FINAL_METHOD
				{
					m_callback_ownerships.mark();
				}
			};

			run::value net_create_io_service(
					run::garbage_collector &gc,
					std::vector<run::value> const &arguments)
			{
				(void)arguments;
				return run::value(run::construct<io_service>(gc));
			}
		}

		run::value register_network_module(run::garbage_collector &gc)
		{
			auto &module = run::construct<run::table>(gc);
			rt::inserter(module, gc)
				.insert_fn("create_io_service", std::bind(net_create_io_service, std::ref(gc), std::placeholders::_1))
				;
			return run::value(module);
		}
	}
}

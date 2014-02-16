#include "tempest_module.hpp"
#include "p0run/garbage_collector.hpp"
#include "p0run/interpreter.hpp"
#include "p0run/table.hpp"
#include "p0common/not_implemented.hpp"
#include "http/http_request.hpp"
#include "p0rt/insert.hpp"
#include <boost/bind.hpp>
#include <boost/unordered_map.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/coroutine/coroutine.hpp>
#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>
#include <boost/function.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace p0
{
	namespace tempest
	{
		namespace
		{
			typedef boost::coroutines::coroutine<void (boost::system::error_code, std::size_t)> coro_t;

			class inbuf : public std::streambuf,
			              private boost::noncopyable
			{
			private:
			    static const std::streamsize        pb_size;

			    enum
			    { bf_size = 16 };

			    int fetch_()
			    {
			        std::streamsize num = std::min(
			            static_cast< std::streamsize >( gptr() - eback() ), pb_size);

			        std::memmove(
			            buffer_ + ( pb_size - num),
			            gptr() - num, num);

			        // read bytes from the socket into internal buffer 'buffer_'
			        // make coro_t::operator() as callback, invoked if some
			        // bytes are read into 'buffer_'
			        s_.async_read_some(
			                boost::asio::buffer( buffer_ + pb_size, bf_size - pb_size),
			                boost::bind( & coro_t::operator(), & coro_, _1, _2) );
			        // suspend this coroutine
			        ca_();

			        // coroutine was resumed by boost::asio::io_sevice
			        boost::system::error_code ec;
			        std::size_t n = 0;

			        // check arguments
			        boost::tie( ec, n) = ca_.get();

			        // check if an error occurred
			        if ( ec)
			        {
			            setg( 0, 0, 0);
			            return -1;
			        }

			        setg( buffer_ + pb_size - num, buffer_ + pb_size, buffer_ + pb_size + n);
			        return n;
			    }

			    boost::asio::ip::tcp::socket      &   s_;
			    coro_t                            &   coro_;
			    coro_t::caller_type               &   ca_;
			    char                                  buffer_[bf_size];

			protected:
			    virtual int underflow()
			    {
			        if ( gptr() < egptr() )
			            return traits_type::to_int_type( * gptr() );

			        if ( 0 > fetch_() )
			            return traits_type::eof();
			        else
			            return traits_type::to_int_type( * gptr() );
			    }

			public:
			    inbuf(
			            boost::asio::ip::tcp::socket & s,
			            coro_t & coro,
			            coro_t::caller_type & ca) :
			        s_( s), coro_( coro), ca_( ca), buffer_()
			    { setg( buffer_ + 4, buffer_ + 4, buffer_ + 4); }
			};
			const std::streamsize inbuf::pb_size = 4;
		}

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

		typedef boost::function<void (::tempest::http_request const &)> request_handler;

		struct session : boost::enable_shared_from_this<session>
		{
			boost::asio::ip::tcp::socket socket;
			coro_t coroutine;
			request_handler const &on_request;

			explicit session(boost::asio::io_service &io, request_handler const &on_request)
				: socket(io)
				, on_request(on_request)
			{
			}

			void start()
		    {
		        coroutine = coro_t(boost::bind(&session::handle_read, shared_from_this(), _1));
		    }

			void handle_read(coro_t::caller_type &ca)
			{
		        inbuf buf(socket, coroutine, ca);
		        std::istream in(&buf);
				auto const request = ::tempest::parse_request(in);
				on_request(request);
		    }
		};

		struct http_server : p0::run::object
		{
			explicit http_server(
					boost::uint16_t port,
					request_handler on_request)
			    : m_acceptor(m_io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
			    , m_on_request(boost::move(on_request))
			{
				begin_accept();
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
			boost::asio::ip::tcp::acceptor m_acceptor;
			request_handler m_on_request;

			virtual void mark_recursively() PROTOLANG0_OVERRIDE
			{
			}

			void begin_accept()
			{
				auto const session_ = boost::make_shared<session>(m_io_service, m_on_request);
				m_acceptor.async_accept(session_->socket, boost::bind(&http_server::on_client, this, session_, boost::asio::placeholders::error));
			}

			void on_client(boost::shared_ptr<session> session, boost::system::error_code const &error)
			{
				if (error)
				{
					//TODO
					return;
				}
				session->start();
				begin_accept();
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
									  run::interpreter &interpreter)
		{
			if (arguments.size() < 2)
			{
				return run::value();
			}
			auto const port = static_cast<boost::uint16_t>(run::to_integer(arguments[0]));
			auto const callback = arguments[1];
			//TODO: mark the callback when collecting garbage!
			auto const on_request = [callback, &interpreter](::tempest::http_request const &request)
			{
				auto &gc = interpreter.garbage_collector();
				run::object &request_table = run::construct<run::table>(gc);
				rt::insert(request_table, gc, "file", request.file);
				rt::insert(request_table, gc, "method", request.method);
				rt::insert(request_table, gc, "version", request.version);
				run::object &headers_table = run::construct<run::table>(gc);
				BOOST_FOREACH (auto const &header, request.headers)
				{
					rt::insert(headers_table, gc, header.first, header.second);
				}
				rt::insert(request_table, gc, "headers", run::value(headers_table));
				std::vector<run::value> arguments;
				arguments.emplace_back(request_table);
				//TODO: mark request_table when collecting garbage!
				p0::run::call(callback, arguments, interpreter);
				//ignore result
			};
			auto &gc = interpreter.garbage_collector();
			return run::value(run::construct<http_server>(gc, port, on_request));
		}

		run::value register_tempest_module(run::interpreter &interpreter)
		{
			auto &gc = interpreter.garbage_collector();
			auto &module = run::construct<run::table>(gc);
			rt::inserter inserter(module, gc);
			inserter.insert_fn("create_http_server",
			                   boost::bind(create_http_server, _1, boost::ref(interpreter)));
			return run::value(module);
		}
	}
}

#pragma once
#ifndef P0RT_NATIVE_CLASS_HPP
#define P0RT_NATIVE_CLASS_HPP


#include "p0run/string.hpp"
#include <cstddef>
#include <boost/unordered_map.hpp>


namespace p0
{
	namespace rt
	{
		namespace detail
		{
			template <std::size_t ...Indices>
			struct indices
			{
				typedef indices<Indices..., sizeof...(Indices)> next;
			};

			template <std::size_t N>
			struct make_indices
			{
				typedef typename make_indices<N - 1>::type::next type;
			};

			template <>
			struct make_indices<0>
			{
				typedef indices<> type;
			};
		}

		namespace native_object_policies
		{
			template <class Class>
			struct native_class
			{
			public:

				template <class Result, class ...Args>
				void add_method(std::string name,
				                Result (Class::*method)(Args...))
				{
					return add_method<Result, Args...>(
						std::move(name),
						[method](Class &instance, Args ...args)
					{
						return (instance.*method)(std::move(args)...);
					});
				}

				template <class Result, class ...Args>
				void add_method(std::string name,
				                Result (Class::*method)(Args...) const)
				{
					return add_method<Result, Args...>(
						std::move(name),
						[method](Class const &instance, Args ...args)
					{
						return (instance.*method)(std::move(args)...);
					});
				}

				template <class Result, class ...Args, class F>
				void add_method(std::string name,
				                F &&method)
				{
					typedef non_overloaded_method<F, Result, Args...> wrapper_type;
					std::unique_ptr<wrapper_type> method_wrapper(
					            new wrapper_type(std::forward<F>(method)));

					auto const i = m_methods.find(name);
					if (i == m_methods.end())
					{
						m_methods.insert(std::make_pair(std::move(name),
														std::move(method_wrapper)));
					}
					else
					{
						auto &overload = i->second;
						overload->overload(overload,
						                   std::move(method_wrapper),
						                   sizeof...(Args));
					}
				}

				run::value call_method(Class &instance,
				                       std::string const &method_name,
				                       std::vector<run::value> const &arguments,
				                       run::interpreter &interpreter) const
				{
					auto const i = m_methods.find(method_name);
					if (i == m_methods.end())
					{
						throw std::runtime_error("Unknown method " + method_name);
					}
					return i->second->call(instance, arguments, interpreter);
				}

			private:

				struct basic_method
				{
					virtual ~basic_method()
					{
					}

					virtual run::value call(
					        Class &value,
					        std::vector<run::value> const &arguments,
					        run::interpreter &interpreter) const = 0;
					virtual void overload(
					        std::unique_ptr<basic_method> &this_,
					        std::unique_ptr<basic_method> new_method,
					        std::size_t argument_count
					        ) = 0;
				};

				struct overloaded_method : basic_method
				{
					void add_method(std::size_t argument_count,
					                std::unique_ptr<basic_method> method)
					{
						m_overloads.insert(
						    std::make_pair(argument_count, std::move(method)));
					}

					virtual run::value call(
					        Class &value,
					        std::vector<run::value> const &arguments,
					        run::interpreter &interpreter) const PROTOLANG0_OVERRIDE
					{
						return find_overload(arguments.size()).call(
						            value, arguments, interpreter);
					}

					virtual void overload(
					        std::unique_ptr<basic_method> &this_,
					        std::unique_ptr<basic_method> new_method,
					        std::size_t argument_count
					        ) PROTOLANG0_OVERRIDE
					{
						assert(this == this_.get());
						add_method(argument_count, std::move(new_method));
					}

				private:

					typedef boost::unordered_map<std::size_t, std::unique_ptr<basic_method>> overloads;

					overloads m_overloads;

					basic_method const &find_overload(std::size_t argument_count) const
					{
						auto const i = m_overloads.find(argument_count);
						if (i == m_overloads.end())
						{
							//TODO
							throw std::runtime_error(
							            "No overload for this argument count");
						}
						return *i->second;
					}
				};

				template <class F, class Result, class ...Args>
				struct non_overloaded_method : basic_method
				{
					template <class G>
					explicit non_overloaded_method(G &&function)
					    : m_function(std::forward<G>(function))
					{
					}

					virtual run::value call(
					        Class &value,
					        std::vector<run::value> const &arguments,
					        run::interpreter &interpreter) const PROTOLANG0_OVERRIDE
					{
						std::size_t expected_arguments = sizeof...(Args);
						if (arguments.size() < expected_arguments)
						{
							throw std::invalid_argument("More arguments required");
						}
						return do_call(value,
						               arguments,
						               interpreter,
						               typename detail::make_indices<sizeof...(Args)>::type(),
						               boost::is_same<void, Result>());
					}

					virtual void overload(
					        std::unique_ptr<basic_method> &this_,
					        std::unique_ptr<basic_method> new_method,
					        std::size_t argument_count
					        ) PROTOLANG0_OVERRIDE
					{
						//TODO make this method exception-safe
						assert(this == this_.get());
						std::unique_ptr<overloaded_method> methods(
						            new overloaded_method);
						methods->add_method(argument_count, std::move(new_method));
						methods->add_method(sizeof...(Args), std::move(this_));
						this_ = std::move(methods);
					}

				private:

					F const m_function;

					template <class Integer,
					          class IsCompatible = boost::is_integral<Integer>>
					static void convert_argument(Integer &to,
					                             run::value const &from)
					{
						to = static_cast<Integer>(run::to_integer(from));
					}

					template <class A>
					static A get_cpp_argument(run::value const &from)
					{
						A to;
						convert_argument(to, from);
						return to;
					}

					template <class C, std::size_t ...Indices>
					run::value do_call(C &&value,
					                   std::vector<run::value> const &arguments,
							           run::interpreter &,
					                   detail::indices<Indices...>,
					                   boost::true_type) const
					{
						std::size_t expected_arguments = sizeof...(Args);
						assert(arguments.size() >= expected_arguments);
						m_function(value,
						           get_cpp_argument<Args>(arguments[Indices])...);
						return run::value();
					}

					template <class C, std::size_t ...Indices>
					run::value do_call(C &&value,
					                   std::vector<run::value> const &arguments,
							           run::interpreter &,
					                   detail::indices<Indices...>,
					                   boost::false_type) const
					{
						std::size_t expected_arguments = sizeof...(Args);
						assert(arguments.size() >= expected_arguments);
						//TODO support more types
						return run::value(
						    m_function(value,
						               get_cpp_argument<Args>(arguments[Indices])...));
					}
				};

			private:

				typedef boost::unordered_map<std::string,
				                             std::unique_ptr<basic_method>> methods_by_name;

				methods_by_name m_methods;
			};

			template <class Class>
			struct native_class_traits;

			template <class Class>
			struct native_methods
			{
				boost::optional<run::value> call_method(
				        run::object & /*obj*/,
				        Class &instance,
				        run::value const &method_name,
				        std::vector<run::value> const &arguments,
				        run::interpreter &interpreter) const
				{
					native_class<Class> const &class_ =
					        native_class_traits<Class>::description();
					return class_.call_method(instance,
					                          run::expect_string(method_name),
					                          arguments,
					                          interpreter);
				}
			};
		}
	}
}


#endif

#pragma once
#ifndef P0RT_NATIVE_CLASS_HPP
#define P0RT_NATIVE_CLASS_HPP


#include "p0run/string.hpp"
#include "p0run/interpreter.hpp"
#include "p0run/construct.hpp"
#include "p0rt/from_cpp.hpp"
#include "p0rt/to_cpp.hpp"
#include <cstddef>


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
			namespace detail
			{
				template <class Class>
				struct basic_method
				{
					typedef
#ifdef _MSC_VER
					//VC++ 2013 is stupid and somehow cannot insert unique_ptr into unordered_map
					//but we need to do that in native_class, so we use shared_ptr instead.
					std::shared_ptr
#else
					std::unique_ptr
#endif
					<basic_method> stored_method_ptr;

					virtual ~basic_method()
					{
					}

					virtual run::value call(
					        Class &value,
					        std::vector<run::value> const &arguments,
					        run::interpreter &interpreter) const = 0;
					virtual void overload(
							stored_method_ptr &this_,
							stored_method_ptr new_method,
							std::size_t argument_count
					        ) = 0;
				};

				template <class Class>
				struct overloaded_method : basic_method<Class>
				{
					typedef typename basic_method<Class>::stored_method_ptr stored_method_ptr;

					void add_method(std::size_t argument_count, stored_method_ptr method)
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
						stored_method_ptr &this_,
						stored_method_ptr new_method,
						std::size_t argument_count
					        ) PROTOLANG0_OVERRIDE
					{
						assert(this == this_.get());
						add_method(argument_count, std::move(new_method));
					}

				private:

					typedef std::unordered_map<std::size_t, stored_method_ptr> overloads;

					overloads m_overloads;

					basic_method<Class> const &find_overload(std::size_t argument_count) const
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

				template <class Class, class F, class Result, class ...Args>
				struct non_overloaded_method : basic_method<Class>
				{
					typedef typename basic_method<Class>::stored_method_ptr stored_method_ptr;

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
						               typename p0::rt::detail::make_indices<sizeof...(Args)>::type(),
						               boost::is_same<void, Result>());
					}

					virtual void overload(
						stored_method_ptr &this_,
						stored_method_ptr new_method,
						std::size_t argument_count
					        ) PROTOLANG0_OVERRIDE
					{
						//TODO make this method exception-safe
						assert(this == this_.get());
						stored_method_ptr methods(new overloaded_method<Class>);
						auto &overloaded_methods = static_cast<overloaded_method<Class> &>(*methods);
						overloaded_methods.add_method(argument_count, std::move(new_method));
						overloaded_methods.add_method(sizeof...(Args), std::move(this_));
						this_ = std::move(methods);
					}

				private:

					F const m_function;

					template <class A,
					          class a_value = typename std::decay<A>::type>
					static a_value get_cpp_argument(run::value const &from)
					{
						a_value to;
						to_cpp(to, from);
						return to;
					}

					template <class C, std::size_t ...Indices>
					run::value do_call(C &&value,
					                   std::vector<run::value> const &arguments,
							           run::interpreter &,
					                   p0::rt::detail::indices<Indices...>,
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
							           run::interpreter &interpreter,
					                   p0::rt::detail::indices<Indices...>,
					                   boost::false_type) const
					{
						std::size_t expected_arguments = sizeof...(Args);
						assert(arguments.size() >= expected_arguments);
						//TODO support more types
						return from_cpp(
						    m_function(value,
						               get_cpp_argument<Args>(arguments[Indices])...),
					        interpreter);
					}
				};
			}

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
					typedef detail::non_overloaded_method<Class, F, Result, Args...> wrapper_type;
					typename detail::basic_method<Class>::stored_method_ptr method_wrapper(
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

				typedef detail::basic_method<Class> method;
				typedef std::unordered_map<std::string, typename method::stored_method_ptr> methods_by_name;

				methods_by_name m_methods;
			};

			template <class Class>
			struct native_class_traits;

			namespace detail
			{
				template <class Class>
				boost::optional<run::value> call_method(
					Class &instance,
					native_class<Class> const &description,
					run::value const &method_name,
					std::vector<run::value> const &arguments,
					run::interpreter &interpreter)
				{
					return description.call_method(
						instance,
						run::expect_string(method_name),
						arguments,
						interpreter);
				}
			}

			template <class Class>
			struct native_methods_global
			{
				boost::optional<run::value> call_method(
				        run::object & /*obj*/,
				        Class &instance,
				        run::value const &method_name,
				        std::vector<run::value> const &arguments,
				        run::interpreter &interpreter) const
				{
					native_class<Class> const &description =
					        native_class_traits<Class>::description();
					return detail::call_method(
						instance, description,
						method_name, arguments, interpreter);
				}
			};

			template <class Class>
			struct native_methods_from_ctor
			{
				explicit native_methods_from_ctor(native_class<Class> const &description)
					: m_description(&description)
				{
				}

				boost::optional<run::value> call_method(
				        run::object & /*obj*/,
				        Class &instance,
				        run::value const &method_name,
				        std::vector<run::value> const &arguments,
				        run::interpreter &interpreter) const
				{
					assert(m_description);
					return detail::call_method(
						instance, *m_description,
						method_name, arguments, interpreter);
				}

			private:

				native_class<Class> const *m_description;
			};
		}
	}
}


#endif

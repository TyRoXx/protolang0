#pragma once
#ifndef P0RT_NATIVE_OBJECT_HPP
#define P0RT_NATIVE_OBJECT_HPP


#include "p0run/object.hpp"
#include "p0run/construct.hpp"
#include <stdexcept>
#include <memory>
#include <boost/unordered_map.hpp>


namespace p0
{
	namespace rt
	{
		namespace detail
		{
			template <class T>
			struct basic_native_object : run::object
			{
				template <class ...Args>
				explicit basic_native_object(Args &&...args)
				    : m_value(std::forward<Args>(args)...)
				{
				}

				T &value()
				{
					return m_value;
				}

				T const &value() const
				{
					return m_value;
				}

			private:

				T m_value;
			};
		}

		namespace native_object_policies
		{
			struct do_not_mark
			{
				template <class T>
				void mark(T &) const
				{
					//do nothing
				}
			};

			struct no_methods
			{
				template <class Value>
				boost::optional<run::value> call_method(
				        run::object &,
				        Value const &,
				        run::value const &,
				        std::vector<run::value> const &,
				        run::interpreter &) const
				{
					return boost::optional<run::value>();
				}
			};

			template <class Class>
			struct native_methods
			{
				boost::optional<run::value> call_method(
				        run::object &obj,
				        Class const &value,
				        run::value const &method_name,
				        std::vector<run::value> const &arguments,
				        run::interpreter &interpreter) const
				{
					return boost::optional<run::value>();
				}

				template <class Result, class ...Args>
				void add_method(std::string name,
				                Result (Class::*method)(Args...))
				{
					//TODO
				}

			private:

				struct basic_method
				{
					virtual ~basic_method()
					{
					}

					virtual run::value call(
					        Class const &value,
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
					        Class const &value,
					        std::vector<run::value> const &arguments,
					        run::interpreter &) const PROTOLANG0_OVERRIDE
					{
						auto const i = m_overloads.find(arguments.size());
						if (i == m_overloads.end())
						{
							//TODO
							throw std::runtime_error(
							            "No overload for this argument count");
						}
						return i->second->call(value, arguments);
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
				};

				template <class Result, class ...Args>
				struct non_overloaded_method : basic_method
				{
					virtual run::value call(
					        Class const &value,
					        std::vector<run::value> const &arguments,
					        run::interpreter &) const PROTOLANG0_OVERRIDE
					{
						//TODO
						return run::value();
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
				};
			};

			struct print_with_operator
			{
				template <class Object, class Value>
				void print(std::ostream &out,
				           Object const &,
				           Value const &value) const
				{
					out << value;
				}
			};

			struct print_object
			{
				template <class Object, class Value>
				void print(std::ostream &out,
				           Object const &obj,
				           Value const &) const
				{
					obj.run::object::print(out);
				}
			};

			struct compare_object
			{
				template <class Value>
				bool equals(run::object const &left,
				            Value const &,
				            run::object const &right) const
				{
					return left.object::equals(right);
				}

				template <class Value>
				run::comparison_result::Enum compare(
				        run::object const &left,
				        Value const &,
				        run::object const &right) const
				{
					return left.object::compare(right);
				}

				template <class Value>
				run::integer get_hash_code(run::object const &left,
				                           Value const &) const
				{
					return left.object::get_hash_code();
				}
			};

			struct no_elements
			{
				template <class Value>
				boost::optional<run::value> get_element(
				        run::object const & /*obj*/,
				        Value const & /*value*/,
				        run::value const & /*key*/) const
				{
					return boost::optional<run::value>();
				}

				template <class Value>
				boost::optional<run::value> set_element(
				        run::object const & /*obj*/,
				        Value const & /*value*/,
				        run::value const & /*key*/,
				        run::value const & /*value*/) const
				{
					return boost::optional<run::value>();
				}
			};

			struct not_bindable
			{
				template <class Value>
				bool bind(run::object &,
				          Value const &,
				          size_t,
				          run::value const &) const
				{
					return false;
				}

				template <class Value>
				boost::optional<run::value> get_bound(
				        run::object const &,
				        Value const &,
				        size_t) const
				{
					return boost::optional<run::value>();
				}
			};

			struct not_callable
			{
				template <class Value>
				boost::optional<run::value> call(
				        run::object const & /*obj*/,
				        Value const & /*value*/,
				        std::vector<run::value> const & /*arguments*/,
				        run::interpreter & /*interpreter*/) const
				{
					return boost::optional<run::value>();
				}
			};

			struct default_native_object
			        : do_not_mark
			        , no_methods
			        , not_callable
			        , print_object
			        , compare_object
			        , no_elements
			        , not_bindable
			{
			};
		}

		template <class T,
		          class Policies = native_object_policies::default_native_object>
		struct native_object PROTOLANG0_FINAL_CLASS
		        : public detail::basic_native_object<T>
		        , private Policies
		{
			template <class ...Args>
			explicit native_object(Args &&...args)
			    : detail::basic_native_object<T>(std::forward<Args>(args)...)
			{
			}

			virtual boost::optional<run::value> get_element(run::value const &key) const PROTOLANG0_FINAL_METHOD
			{
				return Policies::get_element(*this, this->value(), key);
			}

			virtual bool set_element(run::value const &key, run::value const &value) PROTOLANG0_FINAL_METHOD
			{
				return Policies::set_element(*this, this->value(), key, value);
			}

			virtual run::integer get_hash_code() const PROTOLANG0_FINAL_METHOD
			{
				return Policies::get_hash_code(*this, this->value());
			}

			virtual bool equals(run::object const &other) const PROTOLANG0_FINAL_METHOD
			{
				return Policies::equals(*this, this->value(), other);
			}

			virtual run::comparison_result::Enum compare(run::object const &right) const PROTOLANG0_FINAL_METHOD
			{
				return Policies::compare(*this, this->value(), right);
			}

			virtual boost::optional<run::value> call(
				std::vector<run::value> const &arguments,
				run::interpreter &interpreter) PROTOLANG0_FINAL_METHOD
			{
				return Policies::call(*this, this->value(),
				                      arguments, interpreter);
			}

			virtual boost::optional<run::value> call_method(
					run::value const &method_name,
					std::vector<run::value> const &arguments,
					run::interpreter &interpreter
					) PROTOLANG0_FINAL_METHOD
			{
				return Policies::call_method(*this,
				                             this->value(),
				                             method_name, arguments, interpreter);
			}

			virtual void print(std::ostream &out) const PROTOLANG0_FINAL_METHOD
			{
				Policies::print(out, *this, this->value());
			}

			virtual bool bind(size_t index, run::value const &value) PROTOLANG0_FINAL_METHOD
			{
				return Policies::bind(*this, this->value(), index, value);
			}

			virtual boost::optional<run::value> get_bound(size_t index) const PROTOLANG0_FINAL_METHOD
			{
				return Policies::get_bound(*this, this->value(), index);
			}

		private:

			virtual void mark_recursively() PROTOLANG0_FINAL_METHOD
			{
				Policies::mark(this->value());
			}
		};

		template <class T, class ...Args>
		run::object &construct_native(run::garbage_collector &gc, Args &&...args)
		{
			return run::construct<native_object<T>>(gc,
			                                        std::forward<Args>(args)...);
		}
	}
}


#endif

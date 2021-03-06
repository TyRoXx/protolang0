#include "interpreter.hpp"
#include "table.hpp"
#include "string.hpp"
#include "interpreter_listener.hpp"
#include "function.hpp"
#include "runtime_error_exception.hpp"
#include "garbage_collector.hpp"
#include "construct.hpp"
#include "p0i/function_ref.hpp"
#include "p0common/throw.hpp"
#include <cassert>
#include <climits>
#include <stdexcept>
#include <functional>
#include <boost/static_assert.hpp>
#include <boost/type_traits/make_unsigned.hpp>


namespace p0
{
	namespace run
	{
		namespace
		{
			unsigned flag(unsigned index)
			{
				return (1u << index);
			}

			template <class Container>
			struct pop_guard
			{
				template <class E>
				pop_guard(Container &container, E &&element)
					: m_container(container)
				{
					m_container.push_back(std::forward<E>(element));
				}

				~pop_guard()
				{
					m_container.pop_back();
				}

			private:

				Container &m_container;
			};

			template <class T>
			struct auto_swapper
			{
				auto_swapper(T &variable, T const &temporary_value)
					: m_variable(variable)
					, m_previous_value(temporary_value)
				{
					using std::swap;
					swap(m_variable, m_previous_value);
				}

				~auto_swapper()
				{
					using std::swap;
					swap(m_variable, m_previous_value);
				}

			private:

				T &m_variable;
				T m_previous_value;
			};
		}


		interpreter::interpreter(run::garbage_collector &gc,
								 load_module_function load_module)
			: m_load_module(std::move(load_module))
			, m_gc(gc)
			, m_listener(nullptr)
			, m_next_call_frame(0)
		{
		}

		value interpreter::call(
			intermediate::function_ref const &function,
			value const &current_function,
			const std::vector<value> &arguments)
		{
			get(m_next_call_frame, 0) = current_function;
			for (size_t i = 0; i < arguments.size(); ++i)
			{
				get(m_next_call_frame, 1 + i) = arguments[i];
			}
			native_call(function, m_next_call_frame, arguments.size());
			assert(m_locals.size() > m_next_call_frame);
			return get(m_next_call_frame, 0);
		}

		value interpreter::call(
			intermediate::function_ref const &function,
			const std::vector<value> &arguments
			)
		{
			return call(function,
						value(require_function_ref(function)),
						arguments);
		}

		void interpreter::mark_required_objects()
		{
			mark_values(m_locals);
			mark_values(m_current_function_stack);
		}

		run::garbage_collector &interpreter::garbage_collector() const
		{
			return m_gc;
		}

		void interpreter::set_listener(interpreter_listener *listener)
		{
			m_listener = listener;
		}


		void interpreter::native_call(
			intermediate::function_ref const &function,
			std::size_t arguments_address,
			std::size_t argument_count)
		{
			if (m_listener)
			{
				m_listener->enter_function(arguments_address, argument_count);
			}

			size_t const local_frame = arguments_address;

			pop_guard<decltype(m_current_function_stack)> const
				current_function_stack_entry(
					m_current_function_stack,
					get(local_frame, 0)
				);

			//set missing arguments to null
			for (size_t i = argument_count; i < function.function().parameters(); ++i)
			{
				get(local_frame, 1 + i) = value();
			}

			auto &program = function.origin();
			auto const &code = function.function().body();
			auto current_instr = code.begin();

			auto const raise_error = [&](runtime_error_code::type type)
			{
				run::runtime_error const error(
							type,
							function,
							static_cast<size_t>(current_instr - code.begin()));
				if (m_listener)
				{
					m_listener->on_error(error);
				}
				else
				{
					PROTOLANG0_THROW(run::runtime_error_exception(error));
				}
			};

			while (current_instr != code.end())
			{
				auto const operation = current_instr->type();
				if (m_listener)
				{
					m_listener->before_step(*current_instr);
				}

				auto const &instr_arguments = current_instr->arguments();

				using namespace intermediate::instruction_type;

				BOOST_STATIC_ASSERT(intermediate::instruction_type::count_ == 38);

				switch (operation)
				{
				case nothing:
					{
						break;
					}

				case set_from_constant:
					{
						auto const dest_address = static_cast<size_t>(instr_arguments[0]);
						auto const constant = instr_arguments[1];
						get(local_frame, dest_address) = value(static_cast<integer>(constant));
						break;
					}

				case set_null:
					{
						auto const dest_address = static_cast<size_t>(instr_arguments[0]);
						get(local_frame, dest_address) = value();
						break;
					}

				case set_function:
					{
						auto const dest_address = static_cast<size_t>(instr_arguments[0]);
						auto const function_index = static_cast<size_t>(instr_arguments[1]);
						if (function_index >= program.functions().size())
						{
							raise_error(runtime_error_code::invalid_function_id);
						}
						auto const &function_ptr = program.functions()[function_index];
						intermediate::function_ref const ref(program, function_ptr);
						get(local_frame, dest_address) = value(require_function_ref(ref));
						break;
					}

				case set_string:
					{
						auto const dest_address = static_cast<size_t>(instr_arguments[0]);
						auto const string_id = static_cast<size_t>(instr_arguments[1]);
						if (string_id >= program.strings().size())
						{
							raise_error(runtime_error_code::invalid_string_id);
						}

						auto const string = m_string_literals.require_string(program, string_id);
						get(local_frame, dest_address) = string;
						break;
					}

				case bind:
					{
						auto const closure_address = static_cast<size_t>(instr_arguments[0]);
						auto const bound_index = static_cast<size_t>(instr_arguments[1]);
						auto const source_address = static_cast<size_t>(instr_arguments[2]);
						auto const source = get(local_frame, source_address);
						auto &closure = get(local_frame, closure_address);
						if (closure.type == value_type::function_ptr)
						{
							value const new_closure(
								construct<run::function>(m_gc, *closure.function_ptr));
							closure = new_closure;
						}

						//TODO type check is redundant if previous if was taken
						if ((closure.type != value_type::object) ||
							!closure.obj->bind(bound_index, source))
						{
							raise_error(runtime_error_code::bind_to_non_function);
						}
						break;
					}

				case get_bound:
					{
						auto const closure_address = static_cast<size_t>(instr_arguments[0]);
						auto const bound_index = static_cast<size_t>(instr_arguments[1]);
						auto const dest_address = static_cast<size_t>(instr_arguments[2]);
						auto const closure = get(local_frame, closure_address);

						if (closure.type != value_type::object)
						{
							raise_error(runtime_error_code::no_bound_value);
						}

						auto const bound = closure.obj->get_bound(bound_index);
						if (bound)
						{
							auto &destination = get(local_frame, dest_address);
							destination = *bound;
						}
						else
						{
							raise_error(runtime_error_code::no_bound_value);
						}
						break;
					}

				case current_function:
					{
						auto const dest_address = static_cast<size_t>(instr_arguments[0]);
						get(local_frame, dest_address) = m_current_function_stack.back();
						break;
					}

				case copy:
					{
						auto const dest_address = static_cast<size_t>(instr_arguments[0]);
						auto const source_address = static_cast<size_t>(instr_arguments[1]);
						auto const value = get(local_frame, source_address);
						//make copy because get(..) may reallocate
						get(local_frame, dest_address) = value;
						break;
					}

				case add:
				case sub:
				case mul:
				case div:
				case mod:
				case and_:
				case or_:
				case xor_:
				case shift_left:
				case shift_right:
				case shift_signed:
					{
						auto const dest_address = static_cast<size_t>(instr_arguments[0]);
						auto const source_address = static_cast<size_t>(instr_arguments[1]);
						auto const source = get(local_frame, source_address);
						auto &dest = get(local_frame, dest_address);
						if ((dest.type == value_type::integer) &&
							(source.type == value_type::integer))
						{
							auto const check_divisor =
								[&raise_error](integer dividend, integer divisor)
							{
								if (divisor == 0)
								{
									raise_error(runtime_error_code::division_by_zero);
								}
								if (dividend == std::numeric_limits<integer>::min() &&
									divisor == -1)
								{
									raise_error(runtime_error_code::integer_overflow);
								}
							};
							auto const check_shift_amount = [&raise_error](integer shift)
							{
								integer const integer_size_bits = sizeof(integer) * CHAR_BIT;
								if (shift < 0)
								{
									//TODO allow this?
									raise_error(runtime_error_code::negative_shift);
								}
								if (shift >= integer_size_bits)
								{
									//TODO allow this by modulo-ing?
									raise_error(runtime_error_code::shift_out_of_range);
								}
							};
							auto const checked_add = [&raise_error](integer &left, integer right)
							{
								if (left > 0 &&
									std::numeric_limits<integer>::max() - left < right)
								{
									raise_error(runtime_error_code::integer_overflow);
								}
								if (left < 0 &&
									std::numeric_limits<integer>::min() - left > right)
								{
									raise_error(runtime_error_code::integer_overflow);
								}
								left += right;
							};
							integer &left = dest.i;
							integer const right = source.i;
							switch (operation)
							{
							case add:
								checked_add(left, right);
								break;

							case sub:
								checked_add(left, -right);
								break;

							case mul:
								//TODO: check overflow?
								left *= right;
								break;

							case div:
								check_divisor(left, right);
								left /= right;
								break;

							case mod:
								check_divisor(left, right);
								left %= right;
								break;

							case and_: left &= right; break;
							case or_: left |= right; break;
							case xor_: left ^= right; break;

							case shift_left:
								check_shift_amount(right);
								left <<= right;
								break;

							case shift_right:
							{
								check_shift_amount(right);
								typedef boost::make_unsigned<integer>::type unsigned_integer;
								auto const result =
									static_cast<unsigned_integer>(left) >>
									static_cast<unsigned_integer>(right);
								left = static_cast<integer>(result);
								break;
							}

							case shift_signed:
								check_shift_amount(right);
								//TODO: result is unspecified in C++ if left is negative
								left >>= right;
								break;

							default: assert(nullptr == "missing operation"); break;
							}
						}
						break;
					}

				case not_:
					{
						auto const operand_address = static_cast<size_t>(instr_arguments[0]);
						auto &operand = get(local_frame, operand_address);
						operand = value(static_cast<integer>(!to_boolean(operand)));
						break;
					}

				case invert:
					{
						auto const operand_address = static_cast<size_t>(instr_arguments[0]);
						auto &operand = get(local_frame, operand_address);
						if (operand.type == value_type::integer)
						{
							operand.i = ~operand.i;
						}
						break;
					}

				case negate:
					{
						auto const operand_address = static_cast<size_t>(instr_arguments[0]);
						auto &operand = get(local_frame, operand_address);
						if (operand.type == value_type::integer)
						{
							if (operand.i == std::numeric_limits<integer>::min())
							{
								raise_error(runtime_error_code::integer_overflow);
							}
							operand.i = -operand.i;
						}
						break;
					}

				case equal:
				case not_equal:
				case less:
				case less_equal:
				case greater:
				case greater_equal:
					{
						auto const left_address = static_cast<size_t>(instr_arguments[0]);
						auto const right_address = static_cast<size_t>(instr_arguments[1]);
						auto const right = get(local_frame, right_address);
						auto &left = get(local_frame, left_address);
						auto const status = compare(left, right);
						namespace co = comparison_result;
						static std::array<unsigned, 6> const expected_status =
						{{
							flag(co::equal),
							flag(co::less) | flag(co::greater),
							flag(co::less),
							flag(co::less) | flag(co::equal),
							flag(co::greater),
							flag(co::greater) | flag(co::equal),
						}};
						bool const result = ((flag(status) & expected_status[operation - equal]) != 0);
						left = value(static_cast<integer>(result ? 1 : 0));
						break;
					}

				case intermediate::instruction_type::call:
					{
						auto const arguments_address = static_cast<size_t>(instr_arguments[0]);
						auto const argument_count = static_cast<size_t>(instr_arguments[1]);
						auto const &callee = get(local_frame, arguments_address);
						switch (callee.type)
						{
						case value_type::object:
						{
							std::vector<value> arguments;
							for (size_t i = 0; i < argument_count; ++i)
							{
								arguments.push_back(
									get(local_frame, arguments_address + 1 + i));
							}

							auto_swapper<size_t> const call_frame_guard(
										m_next_call_frame, local_frame + arguments_address);

							auto const result = callee.obj->call(arguments, *this);
							if (!result)
							{
								raise_error(runtime_error_code::call_not_supported);
							}
							get(local_frame, arguments_address) = *result;
							break;
						}

						case value_type::function_ptr:
							native_call(
								*callee.function_ptr,
								local_frame + arguments_address,
								argument_count);
							break;

						default:
							raise_error(runtime_error_code::call_non_object);
						}
						break;
					}

				case call_method:
					{
						auto const arguments_address = static_cast<size_t>(instr_arguments[0]);
						auto const argument_count = static_cast<size_t>(instr_arguments[1]);
						auto const method_name_address = static_cast<size_t>(instr_arguments[2]);
						auto const instance = get(local_frame, arguments_address);
						if (instance.type != value_type::object)
						{
							raise_error(runtime_error_code::call_non_object);
						}

						std::vector<value> arguments;
						for (size_t i = 0; i < argument_count; ++i)
						{
							arguments.push_back(
								get(local_frame, arguments_address + 1 + i));
						}

						auto const method_name = get(local_frame, method_name_address);

						auto_swapper<size_t> const call_frame_guard(
									m_next_call_frame, local_frame + arguments_address);

						auto const result = instance.obj->call_method(
									method_name, arguments, *this);
						if (!result)
						{
							raise_error(runtime_error_code::call_not_supported);
						}
						get(local_frame, arguments_address) = *result;
						break;
					}

				case jump:
					{
						auto const destination = static_cast<size_t>(instr_arguments[0]);
						if (destination > code.size())
						{
							raise_error(runtime_error_code::jump_out_of_range);
						}
						current_instr = (code.begin() + static_cast<ptrdiff_t>(destination));
						--current_instr;
						break;
					}

				case jump_if:
				case jump_if_not:
					{
						auto const destination = static_cast<size_t>(instr_arguments[0]);
						if (destination > code.size())
						{
							raise_error(runtime_error_code::jump_out_of_range);
						}
						auto const condition_address = static_cast<size_t>(instr_arguments[1]);
						auto const &condition = get(local_frame, condition_address);
						if (to_boolean(condition) == (operation == jump_if))
						{
							current_instr = (code.begin() + static_cast<ptrdiff_t>(destination));
							--current_instr;
						}
						break;
					}

				case new_table:
					{
						auto const table_address = static_cast<size_t>(instr_arguments[0]);
						auto &destination = get(local_frame, table_address);
						value const created_table_ptr(
									construct<table>(m_gc));
						destination = created_table_ptr;
						break;
					}

				case set_element:
					{
						auto const table_address = static_cast<size_t>(instr_arguments[0]);
						auto const key_address = static_cast<size_t>(instr_arguments[1]);
						auto const value_address = static_cast<size_t>(instr_arguments[2]);
						auto const table = get(local_frame, table_address);
						auto const key = get(local_frame, key_address);
						auto const value = get(local_frame, value_address);
						if (table.type != value_type::object)
						{
							raise_error(runtime_error_code::set_on_non_object);
						}
						if (!table.obj->set_element(key, value))
						{
							raise_error(runtime_error_code::set_not_supported);
						}
						break;
					}

				case get_element:
					{
						auto const table_address = static_cast<size_t>(instr_arguments[0]);
						auto const key_address = static_cast<size_t>(instr_arguments[1]);
						auto const value_address = static_cast<size_t>(instr_arguments[2]);
						auto const table = get(local_frame, table_address);
						auto const key = get(local_frame, key_address);
						if (table.type != value_type::object)
						{
							raise_error(runtime_error_code::get_from_non_object);
						}
						auto const result = table.obj->get_element(key);
						if (!result)
						{
							raise_error(runtime_error_code::get_not_supported);
						}
						get(local_frame, value_address) = *result;
						break;
					}

				case load_module:
				{
					auto const name_address = static_cast<size_t>(instr_arguments[0]);
					auto const result_address = name_address;
					auto const name_object = get(local_frame, name_address);
					if (name_object.type != value_type::object)
					{
						raise_error(runtime_error_code::invalid_module_name);
					}
					//TODO: something better than dynamic_cast
					string const * const name_string = dynamic_cast<string const *>(name_object.obj);
					if (!name_string)
					{
						raise_error(runtime_error_code::invalid_module_name);
					}
					value result;
					if (m_load_module)
					{
						std::string const &name = name_string->content();
						auto const module_handle = m_load_module(*this, name);
						result = module_handle;
					}
					get(local_frame, result_address) = result;
					break;
				}

				default:
					assert(nullptr == "invalid operation id");
					break;
				}

				++current_instr;
			}

			if (m_listener)
			{
				m_listener->leave_function();
			}

			//pop everything but the return value from the stack
			m_locals.resize(local_frame + 1);
		}

		value &interpreter::get(std::size_t local_frame, std::size_t address)
		{
			auto const absolute_address = local_frame + address;
			if (absolute_address >= m_locals.size())
			{
				m_locals.resize(absolute_address + 1);
			}
			return m_locals[absolute_address];
		}

		intermediate::function_ref const &interpreter::require_function_ref(
				intermediate::function_ref const &function)
		{
			return *m_function_refs.insert(function).first;
		}

		void interpreter::mark_values(std::vector<value> const &values)
		{
			std::for_each(
				values.begin(),
				values.end(),
				[](value const &variable)
			{
				if (variable.type == value_type::object)
				{
					variable.obj->mark();
				}
			});
		}


		boost::optional<value> call(run::value callee,
		                            std::vector<value> const &arguments,
		                            interpreter &interpreter)
		{
			switch (callee.type)
			{
			case value_type::function_ptr:
				return interpreter.call(*callee.function_ptr, callee, arguments);

			case value_type::object:
				return callee.obj->call(arguments, interpreter);

			default:
				return boost::none;
			}
		}
	}
}

#pragma once
#ifndef P0RUN_RUNTIME_ERROR_HPP
#define P0RUN_RUNTIME_ERROR_HPP


#include "p0i/function_ref.hpp"
#include <stdexcept>
#include <array>


namespace p0
{
	namespace run
	{
		namespace runtime_error_code
		{
			enum type
			{
				call_non_object,
				call_not_supported,
				integer_overflow,
				division_by_zero,
				get_from_non_object,
				get_not_supported,
				set_on_non_object,
				set_not_supported,
				jump_out_of_range
			};

			inline std::string const &to_string(type type)
			{
				static std::array<std::string, 9> const strings =
				{{
					 "call_non_object",
					 "call_not_supported",
					 "integer_overflow",
					 "division_by_zero",
					 "get_from_non_object",
					 "get_not_supported",
					 "set_on_non_object",
					 "set_not_supported",
					 "jump_out_of_range"
				}};
				return strings[type];
			}
		}


		struct runtime_error : std::runtime_error
		{
			runtime_error(
					runtime_error_code::type type,
					intermediate::function_ref const &function,
					std::size_t instruction
					);
			runtime_error_code::type type() const;
			intermediate::function_ref const &function() const;
			std::size_t instruction() const;

		private:

			runtime_error_code::type m_type;
			intermediate::function_ref m_function;
			std::size_t m_instruction;
		};
	}
}


#endif

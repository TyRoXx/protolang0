#pragma once
#ifndef P0C_LOCAL_FRAME_HPP
#define P0C_LOCAL_FRAME_HPP


#include "reference.hpp"
#include "source_range.hpp"
#include <map>
#include <string>
#include <vector>


namespace p0
{
	struct loop;
	struct function_generator;

	namespace intermediate
	{
		struct emitter;
	}


	struct local_frame PROTOLANG0_FINAL_CLASS
	{
		explicit local_frame(
			local_frame &function_parent
			);
		explicit local_frame(
			function_generator &function_generator
			);
		reference declare_variable(
			source_range name
			);

		reference require_writeable(
			std::string const &name,
			source_range name_position
			);

		/**
		 * @brief Looks for a local variable and makes it readable if found.
		 *        If the variable is found in an outer function, code is emitted
		 *        to retrieve the value from bound variables.
		 * @param name The name of the variable being looked for.
		 * @param name_position The name's position for helpful error reporting
		 *        if the variable is not found.
		 * @param possible_space The method may put the value here.
		 *        Not required to be valid.
		 * @param emitter Where to put instructions to prepare the value.
		 * @return A reference to the value of the variable. May be invalid if
		 *         'possible_space' was invalid.
		 * @throws If the variable is not found, a descriptive error is thrown.
		 */
		reference emit_read_only(
			std::string const &name,
			source_range name_position,
			reference possible_space,
			intermediate::emitter &emitter
			);

		reference allocate(local_address count);
		void deallocate_top(local_address count);
		loop *enter_loop(loop &loop);
		void leave_loop(loop *previous);
		loop *get_loop() const;

	private:

		typedef std::map<std::string, reference> symbols_by_name;


		local_frame * const m_function_parent;
		function_generator &m_function_generator;
		symbols_by_name m_symbols_by_name;
		local_address m_next_local_address;
		loop *m_current_loop;


		local_frame *outer_function_inner_frame() const;
		void init_variables();
		reference find_function_local_variable(std::string const &name);
	};
}


#endif

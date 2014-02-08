#pragma once
#ifndef P0OPTIMIZE_LOCAL_CONSTANT_TRACKER_HPP
#define P0OPTIMIZE_LOCAL_CONSTANT_TRACKER_HPP


#include "p0i/instruction.hpp"
#include <boost/optional.hpp>
#include <unordered_set>
#include <unordered_map>


namespace p0
{
	struct local_constant_tracker
	{
		void handle_incoming_jump();
		void handle_instruction(intermediate::instruction const &instruction);
		boost::optional<integer> find_current_value(local_address where);

	private:

		std::unordered_map<local_address, integer> m_known_values;
	};
}


#endif

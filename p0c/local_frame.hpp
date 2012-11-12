#pragma once
#ifndef P0C_LOCAL_FRAME_HPP
#define P0C_LOCAL_FRAME_HPP


#include "reference.hpp"
#include "source_range.hpp"
#include <map>
#include <string>


namespace p0
{
	struct local_frame
	{
		explicit local_frame(
			local_frame const *parent
			);
		reference declare_variable(
			source_range name
			);
		reference require_symbol(
			source_range name
			) const;
		reference allocate(size_t count);
		void deallocate_top(size_t count);

	private:

		typedef std::map<std::string, reference> symbols_by_name;


		local_frame const * const m_parent;
		symbols_by_name m_symbols_by_name;
		size_t m_next_local_address;
	};
}


#endif

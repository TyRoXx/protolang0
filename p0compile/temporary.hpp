#pragma once
#ifndef P0C_TEMPORARY_HPP
#define P0C_TEMPORARY_HPP


#include "reference.hpp"


namespace p0
{
	struct local_frame;


	struct temporary PROTOLANG0_FINAL_CLASS
	{
		explicit temporary(
			local_frame &frame,
			local_address size
			);
		~temporary();
		reference address() const;
		local_address size() const;

	private:

		local_frame &frame;
		reference const m_address;
		local_address const m_size;
	};
}


#endif

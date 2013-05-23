#pragma once
#ifndef P0C_TEMPORARY_HPP
#define P0C_TEMPORARY_HPP


#include "reference.hpp"
#include "p0common/noexcept.hpp"


namespace p0
{
	struct local_frame;


	struct temporary PROTOLANG0_FINAL_CLASS
	{
		explicit temporary(
			local_frame &frame,
			local_address size
			) PROTOLANG0_NOEXCEPT;
		~temporary() PROTOLANG0_NOEXCEPT;
		reference address() const PROTOLANG0_NOEXCEPT;
		local_address size() const PROTOLANG0_NOEXCEPT;

	private:

		local_frame &frame;
		reference const m_address;
		local_address const m_size;
	};
}


#endif

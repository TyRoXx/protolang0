#pragma once
#ifndef P0C_REFERENCE_HPP
#define P0C_REFERENCE_HPP


#include "p0common/final.hpp"
#include "p0common/types.hpp"


namespace p0
{
	struct reference PROTOLANG0_FINAL_CLASS
	{
		reference();
		explicit reference(
			p0::local_address local_address
			);
		p0::local_address local_address() const;
		bool is_valid() const;

	private:

		p0::local_address m_local_address;
	};


	bool operator == (reference const &left, reference const &right);
}


#endif

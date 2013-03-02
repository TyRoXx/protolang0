#pragma once
#ifndef P0C_REFERENCE_HPP
#define P0C_REFERENCE_HPP


#include "p0common/final.hpp"
#include <cstddef>


namespace p0
{
	struct reference PROTOLANG0_FINAL_CLASS
	{
		reference();
		explicit reference(
			size_t local_address
			);
		size_t local_address() const;
		bool is_valid() const;

	private:

		size_t m_local_address;
	};
}


#endif

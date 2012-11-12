#pragma once
#ifndef P0C_REFERENCE_HPP
#define P0C_REFERENCE_HPP


#include <cstddef>


namespace p0
{
	struct reference
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

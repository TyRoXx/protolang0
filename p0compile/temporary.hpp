#pragma once
#ifndef P0C_TEMPORARY_HPP
#define P0C_TEMPORARY_HPP


#include "reference.hpp"


namespace p0
{
	struct local_frame;


	struct temporary
	{
		explicit temporary(
			local_frame &frame,
			size_t size
			);
		~temporary();
		reference address() const;
		size_t size() const;

	private:

		local_frame &frame;
		reference const m_address;
		size_t const m_size;
	};
}


#endif

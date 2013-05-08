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
			std::size_t size
			);
		~temporary();
		reference address() const;
		std::size_t size() const;

	private:

		local_frame &frame;
		reference const m_address;
		std::size_t const m_size;
	};
}


#endif

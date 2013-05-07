#pragma once
#ifndef P0COMMON_THROW_HPP
#define P0COMMON_THROW_HPP


#include <stdexcept>
#include <type_traits>


namespace p0
{
	template <class Exception>
	typename std::enable_if<std::is_base_of<std::exception, Exception>::value, void>::type
	throw_impl(Exception &&ex)
	{
		throw std::forward<Exception>(ex);
	}
}


#define PROTOLANG0_THROW(ex) throw_impl(ex)


#endif

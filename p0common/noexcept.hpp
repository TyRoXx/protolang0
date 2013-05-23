#pragma once
#ifndef P0COMMON_NOEXCEPT_HPP
#define P0COMMON_NOEXCEPT_HPP


#ifdef _MSC_VER

#	define PROTOLANG0_NOEXCEPT

#elif defined (__clang__)

#	define PROTOLANG0_NOEXCEPT noexcept

#elif defined (__GNUC__)

//GCC 4.6 supports noexcept (http://gcc.gnu.org/gcc-4.6/changes.html)
#	define PROTOLANG0_NOEXCEPT noexcept

#else
#	error Could not detect compiler to apply some workarounds
#endif


#endif

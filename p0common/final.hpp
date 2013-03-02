#pragma once
#ifndef P0COMMON_FINAL_HPP
#define P0COMMON_FINAL_HPP


#ifdef _MSC_VER

//currently only VC++ 2010 is supported, and it knows override but not final
#	define PROTOLANG0_FINAL_METHOD override
#	define PROTOLANG0_FINAL_CLASS

#elif defined (__clang__)

//override and final supported since Clang 3.0 (http://clang.llvm.org/cxx_status.html)
#if (__clang_major__ >= 3)
#	define PROTOLANG0_FINAL_METHOD final
#	define PROTOLANG0_FINAL_CLASS final
#endif

#elif defined (__GNUC__)

//GCC 4.7 supports final
#	if (__GNUC__ >= 4) && ((__GNUC__ > 4) || (__GNUC_MINOR__ >= 7))
#		define PROTOLANG0_FINAL_METHOD final
#		define PROTOLANG0_FINAL_CLASS final
#	else
#		define PROTOLANG0_FINAL_METHOD
#		define PROTOLANG0_FINAL_CLASS
#	endif

#else
#	error Could not detect compiler to apply some workarounds
#endif


#endif

#pragma once
#ifndef P0COMMON_FINAL_HPP
#define P0COMMON_FINAL_HPP


#ifdef _MSC_VER

#	define PROTOLANG0_FINAL_METHOD override

#elif defined (__GNUC__)

#	if (__GNUC__ >= 4) && ((__GNUC__ > 4) || (__GNUC_MINOR__ >= 7))
#		define PROTOLANG0_FINAL_METHOD final
#	else
#		define PROTOLANG0_FINAL_METHOD
#	endif

#else
#	error Could not detect compiler to apply some workarounds
#endif


#endif

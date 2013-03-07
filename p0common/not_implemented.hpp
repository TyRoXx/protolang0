#pragma once
#ifndef P0COMMON_NOT_IMPLEMENTED_HPP
#define P0COMMON_NOT_IMPLEMENTED_HPP



#include <stdexcept>


#define PROTOLANG0_STR(x) PROTOLANG0_DO_STR(x)
#define PROTOLANG0_DO_STR(x) #x


#define P0_NOT_IMPLEMENTED() \
	throw std::runtime_error("Not implemented (" __FILE__ ", " PROTOLANG0_STR(__LINE__) ")")


#endif

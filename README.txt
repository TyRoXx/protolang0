This is a project for learning about compilers and interpreters.
I am designing and implementing a JavaScript-like interpreted programming language.
The name "protolang0" (or p0 for short) indicates that this is my first "serious" attempt at making
a prototypical language. p0 is designed to be as simple as possible while offering all required
features for programming in it. The focus is on clarity and correctness of the C++ implementation.

The License can be found in MIT_LICENSE.txt.
Feel free to ask me anything or contribute.

Dependencies:
	- CMake 2.8+
	- Boost 1.48+ test and program_options
		if CMake does not find Boost, set -DBOOST_ROOT= appropriately

Usually working compilers:
	- GCC 4.7 (also MinGW on Windows)

Good chances to work, but tested infrequently:
	- Clang 3.2

Currently unsupported compilers due to obscure errors or missing C++11 features:
	- Visual C++ 2010

TyRoXx
tyroxxxx@gmail.com
https://github.com/tyroxx

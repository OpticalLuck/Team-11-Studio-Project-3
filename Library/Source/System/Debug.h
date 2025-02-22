#include <iostream>

#ifdef _DEBUG
	#define DEBUG_MSG(str) do { std::cout << "LOG: " << str << std::endl; } while( false )
	#define ASSERT(condition, str) do { if (!condition) std::cout << "ASSERT_ERROR: " << str << std::endl; } while( false )
#else
	#define DEBUG_MSG(str) do { } while ( false ) //Forces a ';' at the end of the macro call.
	#define ASSERT(str) do { } while ( false )
#endif
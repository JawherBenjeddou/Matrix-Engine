#pragma once
#pragma warning (disable:4251)
#include "pch.h"

#ifdef MX_PLATFORM_WINDOWS
	#ifdef MX_BUILD_DLL
		#define MATRIX_DLL __declspec(dllexport)
	#else
		#define MATRIX_DLL __declspec(dllimport)
	#endif // MX_BUILD_DLL
#else
	#error Engine is only supported by windows check rep for more info !
#endif

#ifdef MX_DEBUG_MODE
	#define MX_ASSERT(condition, message) \
	    do { \
	        if (!(condition)) { \
	            std::cerr << "Assertion failed: " << #condition << " in " << __FILE__ << " line " << __LINE__ << ": " << message << std::endl; \
	            std::terminate(); \
	        } \
	    } while (false)
	
#elif MX_RELEASE_MODE
	// In release mode,MX_ASSERT is a no-op.
	#define MX_ASSERT(condition, message) do {} while (false)
#endif
#ifdef MX_DEBUG_MODE
#define MX_ASSERT_EQ(actual, expected, message) \
    MX_ASSERT((actual) == (expected), message)

#define MX_ASSERT_NEQ(actual, notExpected, message) \
    MX_ASSERT((actual) != (notExpected), message)

#define MX_ASSERT_NULL(pointer, message) \
    MX_ASSERT((pointer) == nullptr, message)

#define MX_ASSERT_NOT_NULL(pointer, message) \
    MX_ASSERT((pointer) != nullptr, message)

#else

// In release mode,assert macros are no-ops.
#define MX_ASSERT_EQ(actual, expected, message) do {} while (false)
#define MX_ASSERT_NEQ(actual, notExpected, message) do {} while (false)
#define MX_ASSERT_NULL(pointer, message) do {} while (false)
#define MX_ASSERT_NOT_NULL(pointer, message) do {} while (false)

#endif
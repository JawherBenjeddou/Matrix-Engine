#pragma once
#pragma warning (disable:4251)
#include "pch.h"


#ifdef _WIN64
	#ifdef MX_BUILD_DLL
		#define MATRIX_DLL __declspec(dllexport)
	#else
		#define MATRIX_DLL __declspec(dllimport)
	#endif // MX_BUILD_DLL
#else
	#error Engine is only supported by windows check rep for more info !
#endif

//STATIC == Compile time.
#ifdef MX_DEBUG_MODE
	#define _ASSERT_GLUE(a, b) a ## b
	#define ASSERT_GLUE(a, b) _ASSERT_GLUE(a, b)
	#ifdef __cplusplus
		#if __cplusplus >= 201103L
			#define MX_STATIC_ASSERT(expr) \
			static_assert(expr, \
			"static assert failed:" \
			#expr)
		#else
			// declare a template but only define the
			// true case (via specialization)
			template<bool> class TStaticAssert;
			template<> class TStaticAssert<true> {};
			#define MX_STATIC_ASSERT(expr) \
			enum \
			{ \
			ASSERT_GLUE(g_assert_fail_, __LINE__) \
			= sizeof(TStaticAssert<!!(expr)>) \
			}
		#endif
	#endif
	#define MX_ASSERT(condition, message) \
	    do { \
	        if (!(condition)) { \
	            std::cerr << "Assertion failed: " << #condition << " in " << __FILE__ << " line " << __LINE__ << ": " << message << std::endl; \
	            std::terminate(); \
	        } \
	    } while (false)
	#define MX_ASSERT_EQ(actual, expected, message) \
	    MX_ASSERT((actual) == (expected), message)
	
	#define MX_ASSERT_NEQ(actual, notExpected, message) \
	    MX_ASSERT((actual) != (notExpected), message)
	
	#define MX_ASSERT_NULL(pointer, message) \
	    MX_ASSERT((pointer) == nullptr, message)
	
	#define MX_ASSERT_NOT_NULL(pointer, message) \
	    MX_ASSERT((pointer) != nullptr, message)

#elif MX_RELEASE_MODE
	// In release mode,assert macros are a no-op.
	#define MX_STATIC_ASSERT(expr) ((void)0)
	#define MX_ASSERT(condition, message) ((void)0)
	#define MX_ASSERT_EQ(actual, expected, message) ((void)0)
	#define MX_ASSERT_NEQ(actual, notExpected, message) ((void)0)
	#define MX_ASSERT_NULL(pointer, message) ((void)0)
	#define MX_ASSERT_NOT_NULL(pointer, message) ((void)0)
#endif

namespace Matrix
{
	// custom aliases for smart pointers
	template <typename T>
	using ExclObj = std::unique_ptr<T>;

	template <typename T>
	using SharedObj = std::shared_ptr<T>;
}


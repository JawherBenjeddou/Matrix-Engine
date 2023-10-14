#pragma once
#pragma warning (disable:4251)

#ifdef MX_PLATFORM_WINDOWS
	#ifdef MX_BUILD_DLL
		#define MATRIX_API __declspec(dllexport)
	#else
		#define MATRIX_API __declspec(dllimport)
	#endif // MX_BUILD_DLL
#else
	#error Engine is only supported by windows check rep for more info !
#endif
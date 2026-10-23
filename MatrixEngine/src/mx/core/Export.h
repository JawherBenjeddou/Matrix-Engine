#pragma once
///@file Export.h  DLL import/export macro. Kept separate so low-level headers (e.g. math) can use it without Common.h.

#ifdef _WIN64
    #ifdef MX_BUILD_DLL
        #define MATRIX_DLL __declspec(dllexport)
    #else
        #define MATRIX_DLL __declspec(dllimport)
    #endif
#else
    #error Matrix Engine currently supports 64-bit Windows only.
#endif

#pragma once
#include "Common.h"
///@file DebugContext.h  OpenGL debug-output hookup (active in debug builds).

namespace Matrix::graphics
{
    /// Enables KHR_debug message output and routes messages to the engine logger. No-op in release builds.
    MATRIX_DLL void EnableDebugContext();

    /// Disables the OpenGL debug output.
    MATRIX_DLL void DisableDebugContext();
}

#pragma once
///@file Common.h  Export macros, smart-pointer aliases and assertion helpers shared by the whole engine.

#include "pch.h"
#include "core/Export.h"
#include "core/Version.h"

namespace Matrix
{
    /// Exclusively-owned object (unique_ptr alias kept for API compatibility).
    template <typename T>
    using ExclObj = std::unique_ptr<T>;

    /// Shared object (shared_ptr alias kept for API compatibility).
    template <typename T>
    using SharedObj = std::shared_ptr<T>;

    template <typename T, typename... Args>
    constexpr ExclObj<T> CreateExcl(Args&&... args) { return std::make_unique<T>(std::forward<Args>(args)...); }

    template <typename T, typename... Args>
    constexpr SharedObj<T> CreateShared(Args&&... args) { return std::make_shared<T>(std::forward<Args>(args)...); }

    namespace core
    {
        /// Reports a failed assertion through the logging system (falls back to stderr) and breaks into the debugger if one is attached.
        MATRIX_DLL void ReportAssertionFailure(const char* expression, const char* file, int line, const std::string& message, bool fatal);

        template <typename... Args>
        std::string FormatAssertMessage(Args&&... args)
        {
            std::ostringstream stream;
            ((stream << args), ...);
            return stream.str();
        }
    }
}

#define MX_STATIC_ASSERT(expr) static_assert(expr, "static assert failed: " #expr)

/// Verification macro that stays active in every build: logs an error (and breaks in debug) but lets execution continue.
#define MX_VERIFY(condition, ...) \
    do { \
        if (!(condition)) { \
            ::Matrix::core::ReportAssertionFailure(#condition, __FILE__, __LINE__, ::Matrix::core::FormatAssertMessage(__VA_ARGS__), false); \
        } \
    } while (false)

#ifdef MX_DEBUG_MODE
    /// Debug-only assertion. Terminates the program when the condition fails.
    #define MX_ASSERT(condition, ...) \
        do { \
            if (!(condition)) { \
                ::Matrix::core::ReportAssertionFailure(#condition, __FILE__, __LINE__, ::Matrix::core::FormatAssertMessage(__VA_ARGS__), true); \
            } \
        } while (false)
#else
    #define MX_ASSERT(condition, ...) ((void)0)
#endif

#define MX_ASSERT_EQ(actual, expected, ...)       MX_ASSERT((actual) == (expected), __VA_ARGS__)
#define MX_ASSERT_NEQ(actual, notExpected, ...)   MX_ASSERT((actual) != (notExpected), __VA_ARGS__)
#define MX_ASSERT_NULL(pointer, ...)              MX_ASSERT((pointer) == nullptr, __VA_ARGS__)
#define MX_ASSERT_NOT_NULL(pointer, ...)          MX_ASSERT((pointer) != nullptr, __VA_ARGS__)

#define MX_BIT(x) (1u << (x))

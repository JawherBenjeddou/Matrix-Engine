#pragma once
///@file TestFramework.h  Tiny self-contained test harness (no external dependencies).

#include "Matrix.h"

#include <cstdio>
#include <functional>
#include <string>
#include <vector>

namespace tests
{
    struct TestCase
    {
        std::string Suite;
        std::string Name;
        std::function<void()> Body;
    };

    struct Failure
    {
        std::string Message;
    };

    inline std::vector<TestCase>& Registry()
    {
        static std::vector<TestCase> cases;
        return cases;
    }

    struct Registrar
    {
        Registrar(const char* suite, const char* name, std::function<void()> body)
        {
            Registry().push_back({ suite, name, std::move(body) });
        }
    };

    inline std::string Describe(const char* expr, const char* file, int line, const std::string& detail)
    {
        std::string text = std::string(file) + "(" + std::to_string(line) + "): " + expr;
        if (!detail.empty())
            text += "  [" + detail + "]";
        return text;
    }

    inline bool Approx(float a, float b, float epsilon = 1e-4f) { return std::abs(a - b) <= epsilon; }
    inline bool Approx(const glm::vec2& a, const glm::vec2& b, float epsilon = 1e-4f) { return Approx(a.x, b.x, epsilon) && Approx(a.y, b.y, epsilon); }
    inline bool Approx(const glm::vec3& a, const glm::vec3& b, float epsilon = 1e-4f) { return Approx(a.x, b.x, epsilon) && Approx(a.y, b.y, epsilon) && Approx(a.z, b.z, epsilon); }
    inline bool Approx(const glm::vec4& a, const glm::vec4& b, float epsilon = 1e-4f) { return Approx(a.x, b.x, epsilon) && Approx(a.y, b.y, epsilon) && Approx(a.z, b.z, epsilon) && Approx(a.w, b.w, epsilon); }
}

#define TEST_CASE(suite, name) \
    static void suite##_##name##_body(); \
    static ::tests::Registrar suite##_##name##_registrar(#suite, #name, suite##_##name##_body); \
    static void suite##_##name##_body()

#define CHECK(expr) \
    do { if (!(expr)) throw ::tests::Failure{ ::tests::Describe(#expr, __FILE__, __LINE__, "") }; } while (false)

#define CHECK_MSG(expr, msg) \
    do { if (!(expr)) throw ::tests::Failure{ ::tests::Describe(#expr, __FILE__, __LINE__, (msg)) }; } while (false)

#define CHECK_APPROX(a, b) \
    do { if (!::tests::Approx((a), (b))) throw ::tests::Failure{ ::tests::Describe(#a " ~= " #b, __FILE__, __LINE__, "") }; } while (false)

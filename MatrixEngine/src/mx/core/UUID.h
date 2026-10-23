#pragma once
#include "Common.h"
///@file UUID.h  64-bit random identifiers used to reference entities across copies and files.

namespace Matrix::core
{
    class MATRIX_DLL UUID
    {
    public:
        /// Generates a new random identifier.
        UUID();
        explicit UUID(uint64_t id) : m_ID(id) {}
        UUID(const UUID&) = default;
        UUID& operator=(const UUID&) = default;

        operator uint64_t() const { return m_ID; }
        uint64_t Value() const { return m_ID; }
        bool IsValid() const { return m_ID != 0; }

        static UUID Invalid() { return UUID(0); }

    private:
        uint64_t m_ID;
    };
}

namespace std
{
    template <>
    struct hash<Matrix::core::UUID>
    {
        size_t operator()(const Matrix::core::UUID& id) const noexcept { return std::hash<uint64_t>()(static_cast<uint64_t>(id)); }
    };
}

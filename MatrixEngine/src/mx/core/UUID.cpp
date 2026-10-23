#include "pch.h"
#include "UUID.h"

namespace Matrix::core
{
    namespace
    {
        std::random_device s_RandomDevice;
        std::mt19937_64 s_Engine(s_RandomDevice());
        std::uniform_int_distribution<uint64_t> s_Distribution(1, std::numeric_limits<uint64_t>::max());
    }

    UUID::UUID()
        : m_ID(s_Distribution(s_Engine))
    {
    }
}

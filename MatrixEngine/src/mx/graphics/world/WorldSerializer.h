#pragma once
#include "Common.h"
///@file WorldSerializer.h  Saves and loads worlds as JSON (.mxscene) files.

#include "World.h"

namespace Matrix::graphics
{
    class MATRIX_DLL WorldSerializer
    {
    public:
        explicit WorldSerializer(const SharedObj<World>& world);

        /// Writes the world to a file. Returns false (and logs) on failure.
        bool Serialize(const std::filesystem::path& path);

        /// Loads entities from a file into the world (the world should be empty). Returns false on failure.
        bool Deserialize(const std::filesystem::path& path);

        std::string SerializeToString();
        bool DeserializeFromString(const std::string& json);

        static constexpr int FormatVersion = 1;
        static constexpr const char* Extension = ".mxscene";

    private:
        SharedObj<World> m_World;
    };
}

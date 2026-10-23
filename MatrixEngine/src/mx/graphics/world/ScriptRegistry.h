#pragma once
#include "Common.h"
///@file ScriptRegistry.h  Maps script names (saved in scenes) to C++ factories.

namespace Matrix::graphics
{
    class ScriptableEntity;

    struct MATRIX_DLL ScriptParameterInfo
    {
        std::string Name;
        float DefaultValue = 0.0f;
    };

    class MATRIX_DLL ScriptRegistry
    {
    public:
        using Factory = std::function<ScriptableEntity*()>;

        /// Registers a factory under a name. Re-registering a name replaces the previous entry.
        static void Register(const std::string& name, Factory factory, std::vector<ScriptParameterInfo> parameters = {});

        template <typename T>
        static void Register(const std::string& name, std::vector<ScriptParameterInfo> parameters = {})
        {
            Register(name, []() -> ScriptableEntity* { return new T(); }, std::move(parameters));
        }

        /// Creates a new script instance, or nullptr if the name is unknown.
        static ScriptableEntity* Instantiate(const std::string& name);

        static bool Exists(const std::string& name);
        static std::vector<std::string> GetNames();
        static const std::vector<ScriptParameterInfo>& GetParameters(const std::string& name);
        static void Clear();
    };
}

/// Registers a ScriptableEntity subclass under its class name.
#define MX_REGISTER_SCRIPT(ScriptClass, ...) \
    ::Matrix::graphics::ScriptRegistry::Register<ScriptClass>(#ScriptClass, std::vector<::Matrix::graphics::ScriptParameterInfo>{ __VA_ARGS__ })

#include "pch.h"
#include "ScriptRegistry.h"
#include "ScriptableEntity.h"
#include "core/Logging.h"

namespace Matrix::graphics
{
    namespace
    {
        struct Entry
        {
            ScriptRegistry::Factory Create;
            std::vector<ScriptParameterInfo> Parameters;
        };

        std::map<std::string, Entry>& Registry()
        {
            static std::map<std::string, Entry> registry;
            return registry;
        }

        const std::vector<ScriptParameterInfo>& EmptyParameters()
        {
            static std::vector<ScriptParameterInfo> empty;
            return empty;
        }
    }

    void ScriptRegistry::Register(const std::string& name, Factory factory, std::vector<ScriptParameterInfo> parameters)
    {
        Registry()[name] = Entry{ std::move(factory), std::move(parameters) };
    }

    ScriptableEntity* ScriptRegistry::Instantiate(const std::string& name)
    {
        auto it = Registry().find(name);
        if (it == Registry().end())
        {
            MX_CORE_WARN("Script '{}' is not registered", name);
            return nullptr;
        }
        return it->second.Create();
    }

    bool ScriptRegistry::Exists(const std::string& name)
    {
        return Registry().find(name) != Registry().end();
    }

    std::vector<std::string> ScriptRegistry::GetNames()
    {
        std::vector<std::string> names;
        names.reserve(Registry().size());
        for (const auto& [name, entry] : Registry())
            names.push_back(name);
        return names;
    }

    const std::vector<ScriptParameterInfo>& ScriptRegistry::GetParameters(const std::string& name)
    {
        auto it = Registry().find(name);
        return it == Registry().end() ? EmptyParameters() : it->second.Parameters;
    }

    void ScriptRegistry::Clear()
    {
        Registry().clear();
    }
}

#include "pch.h"
#include "WorldSerializer.h"

#include "core/Logging.h"
#include "util/FileSystem.h"

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace Matrix::graphics
{
    namespace
    {
        json ToJson(const glm::vec2& v) { return json::array({ v.x, v.y }); }
        json ToJson(const glm::vec3& v) { return json::array({ v.x, v.y, v.z }); }
        json ToJson(const glm::vec4& v) { return json::array({ v.x, v.y, v.z, v.w }); }

        glm::vec2 ToVec2(const json& j, const glm::vec2& fallback)
        {
            if (!j.is_array() || j.size() < 2) return fallback;
            return { j[0].get<float>(), j[1].get<float>() };
        }
        glm::vec3 ToVec3(const json& j, const glm::vec3& fallback)
        {
            if (!j.is_array() || j.size() < 3) return fallback;
            return { j[0].get<float>(), j[1].get<float>(), j[2].get<float>() };
        }
        glm::vec4 ToVec4(const json& j, const glm::vec4& fallback)
        {
            if (!j.is_array() || j.size() < 4) return fallback;
            return { j[0].get<float>(), j[1].get<float>(), j[2].get<float>(), j[3].get<float>() };
        }

        template <typename T>
        T Get(const json& j, const char* key, const T& fallback)
        {
            auto it = j.find(key);
            if (it == j.end() || it->is_null())
                return fallback;
            try { return it->get<T>(); }
            catch (...) { return fallback; }
        }

        const char* BodyTypeToString(BodyType2D type)
        {
            switch (type)
            {
            case BodyType2D::Dynamic: return "Dynamic";
            case BodyType2D::Kinematic: return "Kinematic";
            default: return "Static";
            }
        }

        BodyType2D BodyTypeFromString(const std::string& value)
        {
            if (value == "Dynamic") return BodyType2D::Dynamic;
            if (value == "Kinematic") return BodyType2D::Kinematic;
            return BodyType2D::Static;
        }

        json SerializeEntity(Entity entity)
        {
            json e;
            e["ID"] = static_cast<uint64_t>(entity.GetUUID());
            e["Tag"] = entity.GetName();

            {
                const auto& t = entity.GetComponent<TransformComponent>();
                e["TransformComponent"] = { { "Position", ToJson(t.Position) }, { "Rotation", t.Rotation }, { "Scale", ToJson(t.Scale) } };
            }
            if (auto* c = entity.TryGetComponent<RelationshipComponent>())
            {
                json children = json::array();
                for (core::UUID child : c->Children)
                    children.push_back(static_cast<uint64_t>(child));
                e["RelationshipComponent"] = { { "Parent", static_cast<uint64_t>(c->Parent) }, { "Children", children } };
            }
            if (auto* c = entity.TryGetComponent<SpriteComponent>())
                e["SpriteComponent"] = { { "Color", ToJson(c->Color) }, { "TexturePath", c->TexturePath }, { "Tiling", ToJson(c->Tiling) } };
            if (auto* c = entity.TryGetComponent<CircleRendererComponent>())
                e["CircleRendererComponent"] = { { "Color", ToJson(c->Color) }, { "Thickness", c->Thickness }, { "Fade", c->Fade } };
            if (auto* c = entity.TryGetComponent<CameraComponent>())
            {
                e["CameraComponent"] = {
                    { "OrthographicSize", c->Camera.GetOrthographicSize() },
                    { "Near", c->Camera.GetOrthographicNearClip() },
                    { "Far", c->Camera.GetOrthographicFarClip() },
                    { "Primary", c->Primary },
                    { "FixedAspectRatio", c->FixedAspectRatio },
                };
            }
            if (auto* c = entity.TryGetComponent<RigidBody2DComponent>())
            {
                e["RigidBody2DComponent"] = {
                    { "Type", BodyTypeToString(c->Type) },
                    { "Velocity", ToJson(c->Velocity) },
                    { "GravityScale", c->GravityScale },
                    { "Mass", c->Mass },
                    { "Restitution", c->Restitution },
                    { "Friction", c->Friction },
                    { "LinearDamping", c->LinearDamping },
                    { "FixedRotation", c->FixedRotation },
                };
            }
            if (auto* c = entity.TryGetComponent<BoxCollider2DComponent>())
                e["BoxCollider2DComponent"] = { { "Offset", ToJson(c->Offset) }, { "Size", ToJson(c->Size) }, { "IsTrigger", c->IsTrigger } };
            if (auto* c = entity.TryGetComponent<CircleCollider2DComponent>())
                e["CircleCollider2DComponent"] = { { "Offset", ToJson(c->Offset) }, { "Radius", c->Radius }, { "IsTrigger", c->IsTrigger } };
            if (auto* c = entity.TryGetComponent<NativeScriptComponent>())
            {
                json parameters = json::object();
                for (const auto& [name, value] : c->Parameters)
                    parameters[name] = value;
                e["NativeScriptComponent"] = { { "ScriptName", c->ScriptName }, { "Parameters", parameters } };
            }
            if (auto* c = entity.TryGetComponent<AudioSourceComponent>())
            {
                e["AudioSourceComponent"] = {
                    { "ClipPath", c->ClipPath }, { "Volume", c->Volume }, { "Pitch", c->Pitch }, { "Loop", c->Loop }, { "PlayOnStart", c->PlayOnStart }
                };
            }
            return e;
        }

        void DeserializeEntity(World& world, const json& e)
        {
            uint64_t id = Get<uint64_t>(e, "ID", 0);
            std::string tag = Get<std::string>(e, "Tag", "Entity");
            Entity entity = world.CreateEntityWithUUID(core::UUID(id), tag);

            if (auto it = e.find("TransformComponent"); it != e.end())
            {
                auto& t = entity.GetComponent<TransformComponent>();
                t.Position = ToVec3(it->value("Position", json()), t.Position);
                t.Rotation = Get<float>(*it, "Rotation", 0.0f);
                t.Scale = ToVec3(it->value("Scale", json()), t.Scale);
            }
            if (auto it = e.find("RelationshipComponent"); it != e.end())
            {
                auto& c = entity.AddComponent<RelationshipComponent>();
                c.Parent = core::UUID(Get<uint64_t>(*it, "Parent", 0));
                if (auto children = it->find("Children"); children != it->end() && children->is_array())
                    for (const auto& child : *children)
                        if (child.is_number_unsigned())
                            c.Children.push_back(core::UUID(child.get<uint64_t>()));
            }
            if (auto it = e.find("SpriteComponent"); it != e.end())
            {
                auto& c = entity.AddComponent<SpriteComponent>();
                c.Color = ToVec4(it->value("Color", json()), c.Color);
                c.TexturePath = Get<std::string>(*it, "TexturePath", "");
                c.Tiling = ToVec2(it->value("Tiling", json()), glm::vec2(Get<float>(*it, "TilingFactor", 1.0f)));
            }
            if (auto it = e.find("CircleRendererComponent"); it != e.end())
            {
                auto& c = entity.AddComponent<CircleRendererComponent>();
                c.Color = ToVec4(it->value("Color", json()), c.Color);
                c.Thickness = Get<float>(*it, "Thickness", 1.0f);
                c.Fade = Get<float>(*it, "Fade", 0.005f);
            }
            if (auto it = e.find("CameraComponent"); it != e.end())
            {
                auto& c = entity.AddComponent<CameraComponent>();
                c.Camera.SetOrthographic(Get<float>(*it, "OrthographicSize", 10.0f), Get<float>(*it, "Near", -100.0f), Get<float>(*it, "Far", 100.0f));
                c.Primary = Get<bool>(*it, "Primary", true);
                c.FixedAspectRatio = Get<bool>(*it, "FixedAspectRatio", false);
            }
            if (auto it = e.find("RigidBody2DComponent"); it != e.end())
            {
                auto& c = entity.AddComponent<RigidBody2DComponent>();
                c.Type = BodyTypeFromString(Get<std::string>(*it, "Type", "Static"));
                c.Velocity = ToVec2(it->value("Velocity", json()), c.Velocity);
                c.GravityScale = Get<float>(*it, "GravityScale", 1.0f);
                c.Mass = Get<float>(*it, "Mass", 1.0f);
                c.Restitution = Get<float>(*it, "Restitution", 0.0f);
                c.Friction = Get<float>(*it, "Friction", 0.4f);
                c.LinearDamping = Get<float>(*it, "LinearDamping", 0.0f);
                c.FixedRotation = Get<bool>(*it, "FixedRotation", true);
            }
            if (auto it = e.find("BoxCollider2DComponent"); it != e.end())
            {
                auto& c = entity.AddComponent<BoxCollider2DComponent>();
                c.Offset = ToVec2(it->value("Offset", json()), c.Offset);
                c.Size = ToVec2(it->value("Size", json()), c.Size);
                c.IsTrigger = Get<bool>(*it, "IsTrigger", false);
            }
            if (auto it = e.find("CircleCollider2DComponent"); it != e.end())
            {
                auto& c = entity.AddComponent<CircleCollider2DComponent>();
                c.Offset = ToVec2(it->value("Offset", json()), c.Offset);
                c.Radius = Get<float>(*it, "Radius", 0.5f);
                c.IsTrigger = Get<bool>(*it, "IsTrigger", false);
            }
            if (auto it = e.find("NativeScriptComponent"); it != e.end())
            {
                auto& c = entity.AddComponent<NativeScriptComponent>();
                c.ScriptName = Get<std::string>(*it, "ScriptName", "");
                if (auto params = it->find("Parameters"); params != it->end() && params->is_object())
                    for (auto& [name, value] : params->items())
                        if (value.is_number())
                            c.Parameters[name] = value.get<float>();
            }
            if (auto it = e.find("AudioSourceComponent"); it != e.end())
            {
                auto& c = entity.AddComponent<AudioSourceComponent>();
                c.ClipPath = Get<std::string>(*it, "ClipPath", "");
                c.Volume = Get<float>(*it, "Volume", 1.0f);
                c.Pitch = Get<float>(*it, "Pitch", 1.0f);
                c.Loop = Get<bool>(*it, "Loop", false);
                c.PlayOnStart = Get<bool>(*it, "PlayOnStart", true);
            }
        }

        /// Drops dangling parent/child references left by hand-edited or partially loaded files.
        void ValidateHierarchy(World& world)
        {
            for (Entity entity : world.GetAllEntities())
            {
                auto* rel = entity.TryGetComponent<RelationshipComponent>();
                if (!rel)
                    continue;
                if (rel->Parent.IsValid() && !world.GetEntityByUUID(rel->Parent))
                {
                    MX_CORE_WARN("Entity '{}' references a missing parent; detaching", entity.GetName());
                    rel->Parent = core::UUID::Invalid();
                }
                auto& children = rel->Children;
                children.erase(std::remove_if(children.begin(), children.end(), [&](core::UUID id) { return !world.GetEntityByUUID(id); }), children.end());
            }
        }
    }

    WorldSerializer::WorldSerializer(const SharedObj<World>& world)
        : m_World(world)
    {
    }

    std::string WorldSerializer::SerializeToString()
    {
        json root;
        root["Scene"] = m_World->GetName();
        root["Version"] = FormatVersion;
        root["Gravity"] = ToJson(m_World->Gravity);
        root["BackgroundColor"] = ToJson(m_World->BackgroundColor);

        json entities = json::array();
        for (Entity entity : m_World->GetAllEntities())
            entities.push_back(SerializeEntity(entity));
        root["Entities"] = entities;
        return root.dump(2);
    }

    bool WorldSerializer::Serialize(const std::filesystem::path& path)
    {
        std::filesystem::path resolved = util::FileSystem::Resolve(path);
        std::string text;
        try
        {
            text = SerializeToString();
        }
        catch (const std::exception& e)
        {
            MX_CORE_ERROR("Failed to serialize world '{}': {}", m_World->GetName(), e.what());
            return false;
        }
        if (!util::FileSystem::WriteText(resolved, text))
        {
            MX_CORE_ERROR("Could not write scene file '{}'", resolved.string());
            return false;
        }
        m_World->SetFilePath(util::FileSystem::MakeRelative(resolved));
        MX_CORE_INFO("Scene saved to '{}'", m_World->GetFilePath());
        return true;
    }

    bool WorldSerializer::DeserializeFromString(const std::string& text)
    {
        json root;
        try
        {
            root = json::parse(text);
        }
        catch (const std::exception& e)
        {
            MX_CORE_ERROR("Scene parse error: {}", e.what());
            return false;
        }
        if (!root.is_object() || !root.contains("Entities"))
        {
            MX_CORE_ERROR("Scene file is not a Matrix scene (missing 'Entities')");
            return false;
        }

        int version = Get<int>(root, "Version", 1);
        if (version > FormatVersion)
            MX_CORE_WARN("Scene was saved with a newer format (version {}); some data may be ignored", version);

        m_World->SetName(Get<std::string>(root, "Scene", "Untitled"));
        m_World->Gravity = ToVec2(root.value("Gravity", json()), m_World->Gravity);
        m_World->BackgroundColor = ToVec4(root.value("BackgroundColor", json()), m_World->BackgroundColor);

        const json& entities = root["Entities"];
        if (!entities.is_array())
            return false;

        for (const auto& e : entities)
        {
            if (!e.is_object())
                continue;
            try
            {
                DeserializeEntity(*m_World, e);
            }
            catch (const std::exception& ex)
            {
                MX_CORE_ERROR("Skipping malformed entity: {}", ex.what());
            }
        }
        ValidateHierarchy(*m_World);
        return true;
    }

    bool WorldSerializer::Deserialize(const std::filesystem::path& path)
    {
        std::filesystem::path resolved = util::FileSystem::Resolve(path);
        auto text = util::FileSystem::ReadText(resolved);
        if (!text)
        {
            MX_CORE_ERROR("Could not read scene file '{}'", resolved.string());
            return false;
        }
        if (!DeserializeFromString(*text))
            return false;
        m_World->SetFilePath(util::FileSystem::MakeRelative(resolved));
        MX_CORE_INFO("Scene '{}' loaded from '{}' ({} entities)", m_World->GetName(), m_World->GetFilePath(), m_World->GetEntityCount());
        return true;
    }
}

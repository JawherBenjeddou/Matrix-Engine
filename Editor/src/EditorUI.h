#pragma once
///@file EditorUI.h  Reusable ImGui widgets used by the editor panels.

#include "Matrix.h"
#include "imgui.h"

namespace Matrix::editor::ui
{
    /// X/Y/Z drag control with coloured reset buttons. Returns true when a value changed.
    bool DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float speed = 0.05f, float columnWidth = 90.0f);
    bool DrawVec2Control(const std::string& label, glm::vec2& values, float resetValue = 0.0f, float speed = 0.05f, float columnWidth = 90.0f);
    bool DrawFloatControl(const std::string& label, float& value, float speed = 0.05f, float min = 0.0f, float max = 0.0f, float columnWidth = 90.0f, const char* format = "%.3f");
    bool DrawBoolControl(const std::string& label, bool& value, float columnWidth = 90.0f);
    bool DrawColorControl(const std::string& label, glm::vec4& color, float columnWidth = 90.0f);
    bool DrawTextControl(const std::string& label, std::string& value, float columnWidth = 90.0f);
    bool DrawComboControl(const std::string& label, int& current, const char* const* items, int count, float columnWidth = 90.0f);

    /// Draws a path field with a browse button and a content-browser drop target. Returns true when the path changed.
    bool DrawAssetPathControl(const std::string& label, std::string& path, const char* dialogFilter, const char* dropExtensions, float columnWidth = 90.0f);

    /// Accepts a "CONTENT_BROWSER_ITEM" payload whose extension matches one of the given (comma separated) extensions.
    /// Returns the root-relative path when a payload was dropped this frame.
    std::optional<std::string> AcceptAssetDrop(const char* extensions);

    /// Pushes the bold editor font (no-op if unavailable).
    void PushBoldFont();
    void PopBoldFont();

    void HelpMarker(const char* text);

    /// Payload identifier for items dragged out of the content browser (null-terminated root-relative path).
    constexpr const char* ContentBrowserPayload = "CONTENT_BROWSER_ITEM";
    /// Payload identifier for entities dragged inside the hierarchy (uint64 UUID).
    constexpr const char* EntityPayload = "ENTITY_UUID";
}

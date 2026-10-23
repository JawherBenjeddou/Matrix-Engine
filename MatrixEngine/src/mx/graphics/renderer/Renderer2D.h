#pragma once
#include "Common.h"
///@file Renderer2D.h  Batched 2D renderer for textured quads, circles and lines.

#include "graphics/textures/Texture2D.h"

namespace Matrix::graphics
{
    struct MATRIX_DLL Renderer2DStats
    {
        uint32_t DrawCalls = 0;
        uint32_t QuadCount = 0;
        uint32_t CircleCount = 0;
        uint32_t LineCount = 0;

        uint32_t GetTotalVertexCount() const { return QuadCount * 4 + CircleCount * 4 + LineCount * 2; }
        uint32_t GetTotalIndexCount() const { return QuadCount * 6 + CircleCount * 6; }
    };

    /// Static batch renderer. Call Init() once with a live OpenGL context, then wrap draw calls in BeginScene / EndScene.
    class MATRIX_DLL Renderer2D
    {
    public:
        static void Init();
        static void Shutdown();
        static bool IsInitialized();

        static void BeginScene(const glm::mat4& viewProjection);
        static void EndScene();
        static void Flush();

        // --- Quads -------------------------------------------------------------------------
        static void DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);
        /// tiling: how many times the texture repeats on each axis.
        static void DrawQuad(const glm::mat4& transform, const SharedObj<Texture2D>& texture, const glm::vec2& tiling = glm::vec2(1.0f), const glm::vec4& tintColor = glm::vec4(1.0f), int entityID = -1);
        static void DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotationDegrees, const glm::vec4& color);
        static void DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotationDegrees, const SharedObj<Texture2D>& texture, const glm::vec2& tiling = glm::vec2(1.0f), const glm::vec4& tintColor = glm::vec4(1.0f));

        // --- Circles -----------------------------------------------------------------------
        /// thickness: 1 = filled disc, smaller values draw a ring. fade: edge softness.
        static void DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness = 1.0f, float fade = 0.005f, int entityID = -1);

        // --- Lines -------------------------------------------------------------------------
        static void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, int entityID = -1);
        static void DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID = -1);
        static void DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);
        static void SetLineWidth(float width);
        static float GetLineWidth();

        // --- Stats -------------------------------------------------------------------------
        static Renderer2DStats GetStats();
        static void ResetStats();

        static constexpr uint32_t MaxQuads = 10000;
        static constexpr uint32_t MaxTextureSlots = 16;

    private:
        static void StartBatch();
        static void NextBatch();
        static float AssignTextureSlot(const SharedObj<Texture2D>& texture);
    };
}

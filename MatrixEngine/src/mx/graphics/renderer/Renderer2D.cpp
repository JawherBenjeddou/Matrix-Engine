#include "pch.h"
#include "Renderer2D.h"

#include "RenderCommand.h"
#include "core/Logging.h"
#include "graphics/buffers/VertexArray.h"
#include "graphics/shaders/ShaderFactory.h"
#include "graphics/textures/TextureLibrary.h"

#include "GL/glew.h"

namespace Matrix::graphics
{
    namespace
    {
        struct QuadVertex
        {
            glm::vec3 Position;
            glm::vec4 Color;
            glm::vec2 TexCoord;
            float TexIndex;
            glm::vec2 Tiling;
            int EntityID;
        };

        struct CircleVertex
        {
            glm::vec3 WorldPosition;
            glm::vec3 LocalPosition;
            glm::vec4 Color;
            float Thickness;
            float Fade;
            int EntityID;
        };

        struct LineVertex
        {
            glm::vec3 Position;
            glm::vec4 Color;
            int EntityID;
        };

        struct RendererData
        {
            static constexpr uint32_t MaxQuads = Renderer2D::MaxQuads;
            static constexpr uint32_t MaxVertices = MaxQuads * 4;
            static constexpr uint32_t MaxIndices = MaxQuads * 6;
            static constexpr uint32_t MaxLines = 20000;
            static constexpr uint32_t MaxLineVertices = MaxLines * 2;

            SharedObj<VertexArray> QuadVertexArray;
            SharedObj<VertexBuffer> QuadVertexBuffer;
            SharedObj<Shader> QuadShader;

            SharedObj<VertexArray> CircleVertexArray;
            SharedObj<VertexBuffer> CircleVertexBuffer;
            SharedObj<Shader> CircleShader;

            SharedObj<VertexArray> LineVertexArray;
            SharedObj<VertexBuffer> LineVertexBuffer;
            SharedObj<Shader> LineShader;

            SharedObj<Texture2D> WhiteTexture;

            std::vector<QuadVertex> QuadVertices;
            std::vector<CircleVertex> CircleVertices;
            std::vector<LineVertex> LineVertices;
            uint32_t QuadIndexCount = 0;
            uint32_t CircleIndexCount = 0;
            // Core-profile OpenGL only guarantees 1px lines; wider values are deprecated and warn on many drivers.
            float LineWidth = 1.0f;

            std::array<SharedObj<Texture2D>, Renderer2D::MaxTextureSlots> TextureSlots;
            uint32_t TextureSlotIndex = 1; // slot 0 = white texture

            glm::vec4 QuadVertexPositions[4];
            glm::mat4 ViewProjection = glm::mat4(1.0f);
            bool SceneActive = false;

            Renderer2DStats Stats;
        };

        RendererData* s_Data = nullptr;
    }

    bool Renderer2D::IsInitialized()
    {
        return s_Data != nullptr;
    }

    void Renderer2D::Init()
    {
        if (s_Data)
            return;
        s_Data = new RendererData();

        // --- Quads -------------------------------------------------------------------------
        s_Data->QuadVertexArray = CreateShared<VertexArray>();
        s_Data->QuadVertexArray->Init();

        s_Data->QuadVertexBuffer = CreateShared<VertexBuffer>();
        s_Data->QuadVertexBuffer->InitDynamic(RendererData::MaxVertices * sizeof(QuadVertex));
        s_Data->QuadVertexBuffer->SetLayout({
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float4, "a_Color" },
            { ShaderDataType::Float2, "a_TexCoord" },
            { ShaderDataType::Float,  "a_TexIndex" },
            { ShaderDataType::Float2, "a_Tiling" },
            { ShaderDataType::Int,    "a_EntityID" },
        });
        s_Data->QuadVertexArray->AddVertexBuffer(s_Data->QuadVertexBuffer);
        s_Data->QuadVertices.reserve(RendererData::MaxVertices);

        std::vector<uint32_t> quadIndices(RendererData::MaxIndices);
        uint32_t offset = 0;
        for (uint32_t i = 0; i < RendererData::MaxIndices; i += 6)
        {
            quadIndices[i + 0] = offset + 0;
            quadIndices[i + 1] = offset + 1;
            quadIndices[i + 2] = offset + 2;
            quadIndices[i + 3] = offset + 2;
            quadIndices[i + 4] = offset + 3;
            quadIndices[i + 5] = offset + 0;
            offset += 4;
        }
        auto quadIndexBuffer = CreateShared<ElementBuffer>();
        quadIndexBuffer->Init();
        quadIndexBuffer->BufferData(static_cast<uint32_t>(quadIndices.size() * sizeof(uint32_t)), quadIndices.data());
        s_Data->QuadVertexArray->SetElementBuffer(quadIndexBuffer);

        // --- Circles -----------------------------------------------------------------------
        s_Data->CircleVertexArray = CreateShared<VertexArray>();
        s_Data->CircleVertexArray->Init();
        s_Data->CircleVertexBuffer = CreateShared<VertexBuffer>();
        s_Data->CircleVertexBuffer->InitDynamic(RendererData::MaxVertices * sizeof(CircleVertex));
        s_Data->CircleVertexBuffer->SetLayout({
            { ShaderDataType::Float3, "a_WorldPosition" },
            { ShaderDataType::Float3, "a_LocalPosition" },
            { ShaderDataType::Float4, "a_Color" },
            { ShaderDataType::Float,  "a_Thickness" },
            { ShaderDataType::Float,  "a_Fade" },
            { ShaderDataType::Int,    "a_EntityID" },
        });
        s_Data->CircleVertexArray->AddVertexBuffer(s_Data->CircleVertexBuffer);
        s_Data->CircleVertexArray->SetElementBuffer(quadIndexBuffer); // same index pattern
        s_Data->CircleVertices.reserve(RendererData::MaxVertices);

        // --- Lines -------------------------------------------------------------------------
        s_Data->LineVertexArray = CreateShared<VertexArray>();
        s_Data->LineVertexArray->Init();
        s_Data->LineVertexBuffer = CreateShared<VertexBuffer>();
        s_Data->LineVertexBuffer->InitDynamic(RendererData::MaxLineVertices * sizeof(LineVertex));
        s_Data->LineVertexBuffer->SetLayout({
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float4, "a_Color" },
            { ShaderDataType::Int,    "a_EntityID" },
        });
        s_Data->LineVertexArray->AddVertexBuffer(s_Data->LineVertexBuffer);
        s_Data->LineVertices.reserve(RendererData::MaxLineVertices);

        // --- Textures & shaders ------------------------------------------------------------
        s_Data->WhiteTexture = TextureLibrary::Get().GetWhiteTexture();

        auto& shaders = ShaderFactory::GetInstance();
        s_Data->QuadShader = shaders.CreateShader("Renderer2D_Quad", "resources/shaders/Renderer2D_Quad.vert", "resources/shaders/Renderer2D_Quad.frag");
        s_Data->CircleShader = shaders.CreateShader("Renderer2D_Circle", "resources/shaders/Renderer2D_Circle.vert", "resources/shaders/Renderer2D_Circle.frag");
        s_Data->LineShader = shaders.CreateShader("Renderer2D_Line", "resources/shaders/Renderer2D_Line.vert", "resources/shaders/Renderer2D_Line.frag");
        MX_VERIFY(s_Data->QuadShader && s_Data->CircleShader && s_Data->LineShader, "Renderer2D shaders failed to compile; nothing will be drawn");

        if (s_Data->QuadShader)
        {
            int samplers[MaxTextureSlots];
            for (uint32_t i = 0; i < MaxTextureSlots; ++i)
                samplers[i] = static_cast<int>(i);
            s_Data->QuadShader->Bind();
            s_Data->QuadShader->SetIntArray("u_Textures", samplers, MaxTextureSlots);
            s_Data->QuadShader->Unbind();
        }

        s_Data->TextureSlots[0] = s_Data->WhiteTexture;
        s_Data->QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
        s_Data->QuadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
        s_Data->QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
        s_Data->QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

        RenderCommand::Init();
        MX_CORE_INFO("Renderer2D initialized (max {} quads per batch, {} texture slots)", MaxQuads, MaxTextureSlots);
    }

    void Renderer2D::Shutdown()
    {
        delete s_Data;
        s_Data = nullptr;
    }

    void Renderer2D::BeginScene(const glm::mat4& viewProjection)
    {
        MX_ASSERT(s_Data, "Renderer2D::Init() must be called first");
        s_Data->ViewProjection = viewProjection;
        s_Data->SceneActive = true;
        StartBatch();
    }

    void Renderer2D::EndScene()
    {
        if (!s_Data || !s_Data->SceneActive)
            return;
        Flush();
        s_Data->SceneActive = false;
    }

    void Renderer2D::StartBatch()
    {
        s_Data->QuadVertices.clear();
        s_Data->QuadIndexCount = 0;
        s_Data->CircleVertices.clear();
        s_Data->CircleIndexCount = 0;
        s_Data->LineVertices.clear();
        s_Data->TextureSlotIndex = 1;
    }

    void Renderer2D::NextBatch()
    {
        Flush();
        StartBatch();
    }

    void Renderer2D::Flush()
    {
        if (!s_Data)
            return;

        if (s_Data->QuadIndexCount > 0 && s_Data->QuadShader)
        {
            s_Data->QuadVertexBuffer->BufferSubData(0, static_cast<uint32_t>(s_Data->QuadVertices.size() * sizeof(QuadVertex)), s_Data->QuadVertices.data());
            for (uint32_t i = 0; i < s_Data->TextureSlotIndex; ++i)
                s_Data->TextureSlots[i]->Bind(i);

            s_Data->QuadShader->Bind();
            s_Data->QuadShader->SetMat4("u_ViewProjection", s_Data->ViewProjection);
            RenderCommand::DrawIndexed(*s_Data->QuadVertexArray, s_Data->QuadIndexCount);
            ++s_Data->Stats.DrawCalls;
        }

        if (s_Data->CircleIndexCount > 0 && s_Data->CircleShader)
        {
            s_Data->CircleVertexBuffer->BufferSubData(0, static_cast<uint32_t>(s_Data->CircleVertices.size() * sizeof(CircleVertex)), s_Data->CircleVertices.data());
            s_Data->CircleShader->Bind();
            s_Data->CircleShader->SetMat4("u_ViewProjection", s_Data->ViewProjection);
            RenderCommand::DrawIndexed(*s_Data->CircleVertexArray, s_Data->CircleIndexCount);
            ++s_Data->Stats.DrawCalls;
        }

        if (!s_Data->LineVertices.empty() && s_Data->LineShader)
        {
            s_Data->LineVertexBuffer->BufferSubData(0, static_cast<uint32_t>(s_Data->LineVertices.size() * sizeof(LineVertex)), s_Data->LineVertices.data());
            s_Data->LineShader->Bind();
            s_Data->LineShader->SetMat4("u_ViewProjection", s_Data->ViewProjection);
            if (s_Data->LineWidth != 1.0f)
                RenderCommand::SetLineWidth(s_Data->LineWidth);
            RenderCommand::DrawLines(*s_Data->LineVertexArray, static_cast<uint32_t>(s_Data->LineVertices.size()));
            ++s_Data->Stats.DrawCalls;
        }
    }

    float Renderer2D::AssignTextureSlot(const SharedObj<Texture2D>& texture)
    {
        if (!texture || !texture->IsLoaded())
            return 0.0f;

        for (uint32_t i = 1; i < s_Data->TextureSlotIndex; ++i)
        {
            if (s_Data->TextureSlots[i]->GetID() == texture->GetID())
                return static_cast<float>(i);
        }

        if (s_Data->TextureSlotIndex >= MaxTextureSlots)
            NextBatch();

        float index = static_cast<float>(s_Data->TextureSlotIndex);
        s_Data->TextureSlots[s_Data->TextureSlotIndex] = texture;
        ++s_Data->TextureSlotIndex;
        return index;
    }

    void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID)
    {
        DrawQuad(transform, nullptr, glm::vec2(1.0f), color, entityID);
    }

    void Renderer2D::DrawQuad(const glm::mat4& transform, const SharedObj<Texture2D>& texture, const glm::vec2& tiling, const glm::vec4& tintColor, int entityID)
    {
        if (!s_Data || !s_Data->SceneActive)
            return;
        if (s_Data->QuadIndexCount >= RendererData::MaxIndices)
            NextBatch();

        constexpr glm::vec2 texCoords[4] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
        float textureIndex = AssignTextureSlot(texture);

        for (int i = 0; i < 4; ++i)
        {
            QuadVertex vertex;
            vertex.Position = glm::vec3(transform * s_Data->QuadVertexPositions[i]);
            vertex.Color = tintColor;
            vertex.TexCoord = texCoords[i];
            vertex.TexIndex = textureIndex;
            vertex.Tiling = tiling;
            vertex.EntityID = entityID;
            s_Data->QuadVertices.push_back(vertex);
        }
        s_Data->QuadIndexCount += 6;
        ++s_Data->Stats.QuadCount;
    }

    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotationDegrees, const glm::vec4& color)
    {
        DrawQuad(math::ComposeTransform2D(position, rotationDegrees, glm::vec3(size, 1.0f)), color);
    }

    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotationDegrees, const SharedObj<Texture2D>& texture, const glm::vec2& tiling, const glm::vec4& tintColor)
    {
        DrawQuad(math::ComposeTransform2D(position, rotationDegrees, glm::vec3(size, 1.0f)), texture, tiling, tintColor);
    }

    void Renderer2D::DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness, float fade, int entityID)
    {
        if (!s_Data || !s_Data->SceneActive)
            return;
        if (s_Data->CircleIndexCount >= RendererData::MaxIndices)
            NextBatch();

        for (int i = 0; i < 4; ++i)
        {
            CircleVertex vertex;
            vertex.WorldPosition = glm::vec3(transform * s_Data->QuadVertexPositions[i]);
            vertex.LocalPosition = glm::vec3(s_Data->QuadVertexPositions[i]) * 2.0f;
            vertex.Color = color;
            vertex.Thickness = thickness;
            vertex.Fade = fade;
            vertex.EntityID = entityID;
            s_Data->CircleVertices.push_back(vertex);
        }
        s_Data->CircleIndexCount += 6;
        ++s_Data->Stats.CircleCount;
    }

    void Renderer2D::DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, int entityID)
    {
        if (!s_Data || !s_Data->SceneActive)
            return;
        if (s_Data->LineVertices.size() + 2 > RendererData::MaxLineVertices)
            NextBatch();

        s_Data->LineVertices.push_back({ p0, color, entityID });
        s_Data->LineVertices.push_back({ p1, color, entityID });
        ++s_Data->Stats.LineCount;
    }

    void Renderer2D::DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID)
    {
        glm::vec3 p0 = { position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z };
        glm::vec3 p1 = { position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z };
        glm::vec3 p2 = { position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z };
        glm::vec3 p3 = { position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z };
        DrawLine(p0, p1, color, entityID);
        DrawLine(p1, p2, color, entityID);
        DrawLine(p2, p3, color, entityID);
        DrawLine(p3, p0, color, entityID);
    }

    void Renderer2D::DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID)
    {
        glm::vec3 corners[4];
        for (int i = 0; i < 4; ++i)
            corners[i] = glm::vec3(transform * s_Data->QuadVertexPositions[i]);
        for (int i = 0; i < 4; ++i)
            DrawLine(corners[i], corners[(i + 1) % 4], color, entityID);
    }

    void Renderer2D::SetLineWidth(float width)
    {
        if (s_Data)
            s_Data->LineWidth = std::max(width, 1.0f);
    }

    float Renderer2D::GetLineWidth()
    {
        return s_Data ? s_Data->LineWidth : 1.0f;
    }

    Renderer2DStats Renderer2D::GetStats()
    {
        return s_Data ? s_Data->Stats : Renderer2DStats{};
    }

    void Renderer2D::ResetStats()
    {
        if (s_Data)
            s_Data->Stats = Renderer2DStats{};
    }
}

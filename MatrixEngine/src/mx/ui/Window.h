#pragma once
#include "Common.h"
///@file Window.h  GLFW window + OpenGL context owner.

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "Input.h"

namespace Matrix::ui
{
    /// Window creation parameters.
    struct MATRIX_DLL WindowSpec
    {
        std::string Title = "Matrix Engine";
        uint32_t Width = 1600;
        uint32_t Height = 900;
        bool VSync = true;
        bool Resizable = true;
        bool Maximized = false;
        /// Root-relative path to a PNG used as the window icon (empty for none).
        std::string IconPath = "resources/branding/icon/matrix_icon.png";

        WindowSpec() = default;
        WindowSpec(std::string_view title, uint32_t width, uint32_t height)
            : Title(title), Width(width), Height(height) {}
    };

    class MATRIX_DLL Window
    {
    public:
        using ResizeCallback = std::function<void(uint32_t width, uint32_t height)>;

        explicit Window(const WindowSpec& spec = WindowSpec());
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        /// Creates the OS window and OpenGL context. Returns false on failure.
        bool InitSystem();

        /// Destroys the window and terminates GLFW. Safe to call more than once.
        void Shutdown();

        /// Clears transient input state and pumps OS events.
        void PollEvents();

        /// Presents the back buffer.
        void SwapBuffers();

        /// Legacy helper: PollEvents() + SwapBuffers().
        void OnUpdate();

        bool Closed() const;
        bool ShouldClose() const { return Closed(); }
        void Close();

        /// Logs renderer / driver information.
        void GetInfo() const;

        void SetWindowIcon(const std::string& rootRelativePath);
        void SetTitle(const std::string& title);
        void SetVSync(bool enabled);
        bool IsVSync() const { return m_VSync; }
        void Maximize();
        bool IsMinimized() const { return m_Minimized; }

        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }
        float GetAspectRatio() const { return m_Height == 0 ? 1.0f : static_cast<float>(m_Width) / static_cast<float>(m_Height); }

        GLFWwindow* GetNativeWindow() const { return m_Window; }
        GLFWwindow* GetWindow() const { return m_Window; }

        void SetResizeCallback(ResizeCallback callback) { m_ResizeCallback = std::move(callback); }

        /// Reads the current back buffer and writes it to a PNG file. Returns false on failure.
        bool CaptureScreenshot(const std::filesystem::path& path) const;

        const std::string& GetGLRenderer() const { return m_GLRenderer; }
        const std::string& GetGLVersion() const { return m_GLVersion; }

    private:
        bool InitGL();

        static void KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void MouseCallBack(GLFWwindow* window, int button, int action, int mods);
        static void CursorPositionCallBack(GLFWwindow* window, double xpos, double ypos);
        static void CursorEnterCallBack(GLFWwindow* window, int entered);
        static void ScrollCallBack(GLFWwindow* window, double xoffset, double yoffset);
        static void FramebufferSizeCallBack(GLFWwindow* window, int width, int height);
        static void WindowCloseCallBack(GLFWwindow* window);
        static void ErrorCallBack(int error, const char* description);

    private:
        WindowSpec m_Spec;
        std::string m_Title;
        uint32_t m_Width;
        uint32_t m_Height;
        bool m_VSync;
        bool m_Minimized = false;
        bool m_Initialized = false;
        GLFWwindow* m_Window = nullptr;
        ResizeCallback m_ResizeCallback;
        std::string m_GLRenderer;
        std::string m_GLVersion;
    };
}

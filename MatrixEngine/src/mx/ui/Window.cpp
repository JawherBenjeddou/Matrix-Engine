#include "pch.h"
#include "Window.h"

#include "core/Logging.h"
#include "graphics/DebugContext.h"
#include "util/FileSystem.h"

#include "stb_image.h"
#include "stb_image_write.h"

namespace Matrix::ui
{
    namespace
    {
        Window* GetSelf(GLFWwindow* window)
        {
            return static_cast<Window*>(glfwGetWindowUserPointer(window));
        }
    }

    Window::Window(const WindowSpec& spec)
        : m_Spec(spec),
          m_Title(spec.Title),
          m_Width(spec.Width),
          m_Height(spec.Height),
          m_VSync(spec.VSync)
    {
    }

    Window::~Window()
    {
        Shutdown();
    }

    bool Window::InitSystem()
    {
        if (m_Initialized)
            return true;

        glfwSetErrorCallback(Window::ErrorCallBack);
        if (!glfwInit())
        {
            MX_CORE_CRITICAL("Failed to initialize GLFW");
            return false;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, m_Spec.Resizable ? GLFW_TRUE : GLFW_FALSE);
        glfwWindowHint(GLFW_MAXIMIZED, m_Spec.Maximized ? GLFW_TRUE : GLFW_FALSE);
#ifdef MX_DEBUG_MODE
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

        m_Window = glfwCreateWindow(static_cast<int>(m_Width), static_cast<int>(m_Height), m_Title.c_str(), nullptr, nullptr);
        if (!m_Window)
        {
            MX_CORE_CRITICAL("Window creation failed (OpenGL 4.5 core profile required)");
            glfwTerminate();
            return false;
        }

        glfwSetWindowUserPointer(m_Window, this);
        glfwSetKeyCallback(m_Window, Window::KeyCallBack);
        glfwSetMouseButtonCallback(m_Window, Window::MouseCallBack);
        glfwSetCursorPosCallback(m_Window, Window::CursorPositionCallBack);
        glfwSetCursorEnterCallback(m_Window, Window::CursorEnterCallBack);
        glfwSetScrollCallback(m_Window, Window::ScrollCallBack);
        glfwSetFramebufferSizeCallback(m_Window, Window::FramebufferSizeCallBack);
        glfwSetWindowCloseCallback(m_Window, Window::WindowCloseCallBack);

        if (!InitGL())
        {
            glfwDestroyWindow(m_Window);
            m_Window = nullptr;
            glfwTerminate();
            return false;
        }

        // The framebuffer may differ from the requested size (DPI scaling, maximized windows).
        int fbWidth = 0, fbHeight = 0;
        glfwGetFramebufferSize(m_Window, &fbWidth, &fbHeight);
        m_Width = static_cast<uint32_t>(std::max(fbWidth, 0));
        m_Height = static_cast<uint32_t>(std::max(fbHeight, 0));
        glViewport(0, 0, fbWidth, fbHeight);

        SetVSync(m_VSync);
        if (!m_Spec.IconPath.empty())
            SetWindowIcon(m_Spec.IconPath);

        m_Initialized = true;
        MX_CORE_INFO("Window created: '{}' {}x{}", m_Title, m_Width, m_Height);
        return true;
    }

    void Window::Shutdown()
    {
        if (!m_Initialized)
            return;
        if (m_Window)
        {
            glfwDestroyWindow(m_Window);
            m_Window = nullptr;
        }
        glfwTerminate();
        m_Initialized = false;
        MX_CORE_INFO("Window destroyed");
    }

    bool Window::InitGL()
    {
        glfwMakeContextCurrent(m_Window);
        glewExperimental = GL_TRUE;
        GLenum status = glewInit();
        if (status != GLEW_OK)
        {
            MX_CORE_CRITICAL("GLEW initialization failed: {}", reinterpret_cast<const char*>(glewGetErrorString(status)));
            return false;
        }
        // glewInit with glewExperimental can leave a stray GL_INVALID_ENUM; clear it.
        while (glGetError() != GL_NO_ERROR) {}

        const char* renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
        const char* version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
        m_GLRenderer = renderer ? renderer : "Unknown";
        m_GLVersion = version ? version : "Unknown";

        graphics::EnableDebugContext();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_LINE_SMOOTH);
        return true;
    }

    void Window::GetInfo() const
    {
        MX_CORE_INFO("Graphics card : {}", m_GLRenderer);
        MX_CORE_INFO("OpenGL        : {}", m_GLVersion);
        if (glfwExtensionSupported("GL_NVX_gpu_memory_info"))
        {
            GLint totalKB = 0;
            glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &totalKB);
            MX_CORE_INFO("Video memory  : {} MB", totalKB / 1024);
        }
    }

    void Window::PollEvents()
    {
        Input::NewFrame();
        glfwPollEvents();
    }

    void Window::SwapBuffers()
    {
        glfwSwapBuffers(m_Window);
    }

    void Window::OnUpdate()
    {
        PollEvents();
        SwapBuffers();
    }

    bool Window::Closed() const
    {
        return m_Window == nullptr || glfwWindowShouldClose(m_Window);
    }

    void Window::Close()
    {
        if (m_Window)
            glfwSetWindowShouldClose(m_Window, GLFW_TRUE);
    }

    void Window::SetWindowIcon(const std::string& rootRelativePath)
    {
        std::filesystem::path path = util::FileSystem::Resolve(rootRelativePath);
        std::string pathString = path.string();
        int width = 0, height = 0, channels = 0;
        stbi_set_flip_vertically_on_load(false);
        unsigned char* data = stbi_load(pathString.c_str(), &width, &height, &channels, STBI_rgb_alpha);
        if (!data)
        {
            MX_CORE_WARN("Window icon not found: {}", pathString);
            return;
        }
        GLFWimage icon{ width, height, data };
        glfwSetWindowIcon(m_Window, 1, &icon);
        stbi_image_free(data);
    }

    void Window::SetTitle(const std::string& title)
    {
        m_Title = title;
        if (m_Window)
            glfwSetWindowTitle(m_Window, title.c_str());
    }

    void Window::SetVSync(bool enabled)
    {
        m_VSync = enabled;
        glfwSwapInterval(enabled ? 1 : 0);
    }

    void Window::Maximize()
    {
        if (m_Window)
            glfwMaximizeWindow(m_Window);
    }

    bool Window::CaptureScreenshot(const std::filesystem::path& path) const
    {
        if (!m_Window || m_Width == 0 || m_Height == 0)
            return false;

        const int width = static_cast<int>(m_Width);
        const int height = static_cast<int>(m_Height);
        std::vector<unsigned char> pixels(static_cast<size_t>(width) * height * 4);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glReadBuffer(GL_BACK);
        glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

        // Screenshots must be fully opaque regardless of the back buffer alpha.
        for (size_t i = 3; i < pixels.size(); i += 4)
            pixels[i] = 255;

        std::error_code ec;
        if (path.has_parent_path())
            std::filesystem::create_directories(path.parent_path(), ec);

        stbi_flip_vertically_on_write(1);
        std::string pathString = path.string();
        int ok = stbi_write_png(pathString.c_str(), width, height, 4, pixels.data(), width * 4);
        if (ok)
            MX_CORE_INFO("Screenshot saved to {}", pathString);
        else
            MX_CORE_ERROR("Failed to write screenshot to {}", pathString);
        return ok != 0;
    }

    // --- GLFW callbacks ------------------------------------------------------------------

    void Window::KeyCallBack(GLFWwindow*, int key, int, int action, int)
    {
        Input::SetKeyState(key, action);
    }

    void Window::MouseCallBack(GLFWwindow*, int button, int action, int)
    {
        Input::SetMouseState(button, action);
    }

    void Window::CursorPositionCallBack(GLFWwindow*, double xpos, double ypos)
    {
        Input::SetCursorPosition(glm::vec2(static_cast<float>(xpos), static_cast<float>(ypos)));
    }

    void Window::CursorEnterCallBack(GLFWwindow*, int entered)
    {
        Input::CursorFocus(entered);
    }

    void Window::ScrollCallBack(GLFWwindow*, double xoffset, double yoffset)
    {
        Input::AddScroll(glm::vec2(static_cast<float>(xoffset), static_cast<float>(yoffset)));
    }

    void Window::FramebufferSizeCallBack(GLFWwindow* window, int width, int height)
    {
        Window* self = GetSelf(window);
        if (!self)
            return;
        self->m_Width = static_cast<uint32_t>(std::max(width, 0));
        self->m_Height = static_cast<uint32_t>(std::max(height, 0));
        self->m_Minimized = (width == 0 || height == 0);
        if (!self->m_Minimized)
            glViewport(0, 0, width, height);
        if (self->m_ResizeCallback)
            self->m_ResizeCallback(self->m_Width, self->m_Height);
    }

    void Window::WindowCloseCallBack(GLFWwindow*)
    {
        MX_CORE_INFO("Window close requested");
    }

    void Window::ErrorCallBack(int error, const char* description)
    {
        MX_CORE_ERROR("GLFW error {}: {}", error, description ? description : "");
    }
}

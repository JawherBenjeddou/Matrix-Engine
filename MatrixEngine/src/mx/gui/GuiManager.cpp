#include "pch.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "GuiManager.h"

namespace Matrix
{
	namespace gui
	{




		GuiManager::GuiManager(Matrix::ui::Window* window)
			:m_Window(window)
		{

		}

		void GuiManager::InitSystem()
		{

			// Setup Dear ImGui context
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
			// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

			// Setup Dear ImGui style
			ImGui::StyleColorsDark();
			// ImGui::StyleColorsLight();

			// Setup Platform/Renderer backends
			ImGui_ImplGlfw_InitForOpenGL(m_Window->GetWindow(), true);
			ImGui_ImplOpenGL3_Init("#version 460"); // Note: NULL as the GLSL version because GLEW already initializes the GL context.

		}

		void GuiManager::Shutdown()
		{
			// Cleanup
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
		}

		void GuiManager::BeginFrame()
		{
			// Start the Dear ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		}

		void GuiManager::RenderGuiFrame()
		{
			// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
			{
				ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

				ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

				if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
					counter++;
				ImGui::SameLine();
				ImGui::Text("counter = %d", counter);

				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f);
				ImGui::End();
			}
			// Rendering
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

	}
}

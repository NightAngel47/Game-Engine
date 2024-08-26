#include "enginepch.h"
#include "Engine/Core/Input.h"

#include "Engine/Core/Application.h"
#include <GLFW/glfw3.h>
#include <imgui.h>

namespace Engine
{
	bool Input::IsKeyPressed(const KeyCode key)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, static_cast<int32_t>(key));
		return state == GLFW_PRESS;
	}

	bool Input::IsMouseButtonPressed(const MouseCode button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_PRESS;
	}

	glm::vec2 Input::GetMousePosition()
	{
		if (!Application::Get().GetSpecification().Runtime)
		{
			auto [mx, my] = ImGui::GetMousePos();
			glm::vec2 viewportBoundsOrigin = Application::Get().GetImGuiLayer()->GetViewportBounds()[0];
			mx -= viewportBoundsOrigin.x;
			my -= viewportBoundsOrigin.y;
			return { mx, my };
		}

		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		return { xpos, ypos };
	}

	float Input::GetMouseX()
	{
		return GetMousePosition().x;
	}

	float Input::GetMouseY()
	{
		return GetMousePosition().y;
	}
	
}

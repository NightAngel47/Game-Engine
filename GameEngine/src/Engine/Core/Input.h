#pragma once

#include "Engine/Core/KeyCodes.h"
#include "Engine/Core/MouseCodes.h"

#include <glm/vec2.hpp>

namespace Engine
{
	class Input
	{
	public:
		static bool IsKeyPressed(KeyCode key);
		
		static bool IsMouseButtonPressed(MouseCode button);
		static glm::vec2 GetMousePosition();
		static float GetMouseY();
		static float GetMouseX();
	};
}

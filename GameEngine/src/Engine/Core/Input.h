#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Core/KeyCodes.h"
#include "Engine/Core/MouseCodes.h"

namespace Engine
{
	class Input
	{
	protected:
		Input() = default;
	public:
		Input(const Input&) = delete;
		Input& operator=(const Input&) = delete;
		
		inline static bool IsKeyPressed(KeyCode key) { return s_Instance-> IsKeyPressedImpl(key); }
		inline static bool IsKeyReleased(KeyCode key) { return s_Instance-> IsKeyReleasedImpl(key); }
		
		inline static bool IsMouseButtonPressed(MouseCode button) { return s_Instance-> IsMouseButtonPressedImpl(button); }
		inline static bool IsMouseButtonReleased(MouseCode button) { return s_Instance-> IsMouseButtonReleasedImpl(button); }
		inline static std::pair<float, float> GetMousePosition() { return s_Instance-> GetMousePositionImpl(); }
		inline static float GetMouseY() { return s_Instance-> GetMouseYImpl(); }
		inline static float GetMouseX() { return s_Instance-> GetMouseXImpl(); }

		static Scope<Input> Create();
	protected:
		virtual bool IsKeyPressedImpl(KeyCode key) = 0;
		virtual bool IsKeyReleasedImpl(KeyCode key) = 0;
		
		virtual bool IsMouseButtonPressedImpl(MouseCode button) = 0;
		virtual bool IsMouseButtonReleasedImpl(MouseCode button) = 0;
		virtual std::pair<float, float> GetMousePositionImpl() = 0;
		virtual float GetMouseXImpl() = 0;
		virtual float GetMouseYImpl() = 0;
	private:
		static Scope<Input> s_Instance;
	};
}

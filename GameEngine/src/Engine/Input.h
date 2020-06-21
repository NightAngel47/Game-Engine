#pragma once

#include "Engine/Core.h"

namespace Engine
{
	class Input
	{
	public:
		inline static bool IsKeyPressed(int keycode) { return s_Instance-> IsKeyPressedImpl(keycode); }
		inline static bool IsKeyReleased(int keycode) { return s_Instance-> IsKeyReleasedImpl(keycode); }
		
		inline static bool IsMouseButtonPressed(int button) { return s_Instance-> IsMouseButtonPressedImpl(button); }
		inline static bool IsMouseButtonReleased(int button) { return s_Instance-> IsMouseButtonReleasedImpl(button); }
		inline static std::pair<float, float> GetMousePosition() { return s_Instance-> GetMousePositionImpl(); }
		inline static float GetMouseY() { return s_Instance-> GetMouseYImpl(); }
		inline static float GetMouseX() { return s_Instance-> GetMouseXImpl(); }

	protected:
		virtual bool IsKeyPressedImpl(int keycode) = 0;
		virtual bool IsKeyReleasedImpl(int keycode) = 0;
		
		virtual bool IsMouseButtonPressedImpl(int button) = 0;
		virtual bool IsMouseButtonReleasedImpl(int button) = 0;
		virtual std::pair<float, float> GetMousePositionImpl() = 0;
		virtual float GetMouseXImpl() = 0;
		virtual float GetMouseYImpl() = 0;
	private:
		static Input* s_Instance;
	};
	
}
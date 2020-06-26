#pragma once

#include "Engine/Core/Input.h"

namespace Engine
{
	class WindowsInput : public Input
	{
	protected:
		virtual bool IsKeyPressedImpl(KeyCode key) override;
		virtual bool IsKeyReleasedImpl(KeyCode ke) override;

		virtual bool IsMouseButtonPressedImpl(MouseCode button) override;
		virtual bool IsMouseButtonReleasedImpl(MouseCode button) override;
		virtual std::pair<float, float> GetMousePositionImpl() override;
		virtual float GetMouseXImpl() override;
		virtual float GetMouseYImpl() override;
	};
}
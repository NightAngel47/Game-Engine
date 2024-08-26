#pragma once

#include "Engine/Core/Layer.h"

#include <glm/vec2.hpp>

namespace Engine
{
	struct Viewport
	{
		glm::vec2 m_ViewportSize = glm::vec2{ 1.0f };
		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportBounds[2] = {};
	};

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer() = default;
		~ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnEvent(Event& e) override;

		void Begin();
		void End();

		void BlockEvents(bool block) { m_BlockEvents = block; }
		
		void SetDarkThemeColors();

		uint32_t GetActiveWidgetID() const;

		Viewport& GetViewport() { return *m_Viewport; }
		glm::vec2 GetViewportSize() { return m_Viewport->m_ViewportSize; }
		glm::vec2* GetViewportBounds() { return m_Viewport->m_ViewportBounds; }
		bool IsViewportFocused() { return m_Viewport->m_ViewportFocused; }
		bool IsViewportHovered() { return m_Viewport->m_ViewportHovered; }
	private:
		bool m_BlockEvents = true;

		Scope<Viewport> m_Viewport{};
	};
}

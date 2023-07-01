#pragma once
#include <Engine.h>
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Engine/Renderer/EditorCamera.h"

namespace Engine
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer()
			: Layer("EditorLayer") {}
		virtual ~EditorLayer() = default;
		
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		
		virtual void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& event) override;
	private:
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
		bool OnWindowDrop(WindowDropEvent& e);

		void OnOverlayRender();

		void NewProject();
		bool OpenProject();
		void OpenProject(const std::filesystem::path& path);
		void SaveProject();

		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveSceneAs();
		void SaveScene();

		void OnScenePlay();
		void OnSceneSimulate();
		void OnSceneStop();

		void OnDuplicateEntity();

		void MousePicking();

		// UI Panels
		void UI_Toolbar();
	private:
		Ref<Framebuffer> m_Framebuffer;

		//Ref<Scene> m_ActiveScene;
		Ref<Scene> m_EditorScene;
		std::filesystem::path m_EditorScenePath;
		
		UUID m_HoveredEntityID = UUID::INVALID();

		bool m_PrimaryCamera = true;
		EditorCamera m_EditorCamera;

		// Stats
		float m_FrameTime = 0;

		// ImGui
		glm::vec2 m_ViewportSize = glm::vec2{1.0f};
		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportBounds[2] = {};

		// Gizmos
		int m_GizmoType = -1;
		bool m_IsGizmoInUse = false;
		bool m_IsGizmoWorld = true;

		bool m_ShowPhysicsColliders = true;

		bool m_ShowSpriteWindow = false;

		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
		Scope<ContentBrowserPanel> m_ContentBrowserPanel;

		// Editor Resources
		
		Ref<Texture2D> m_IconPlay, m_IconPause, m_IconStep, m_IconSimulate, m_IconStop, m_Outline;
	};

}

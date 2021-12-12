#pragma once
#include <Engine.h>
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Engine/Renderer/EditorCamera.h"
#include "Engine/Events/KeyEvent.h"

namespace Engine
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;
		
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		
		virtual void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& event) override;
	private:
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

		void OnOverlayRender();

		void NewScene(const std::filesystem::path& path = std::string());
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveSceneAs();
		void SaveScene();

		void OnScenePlay();
		void OnSceneStop();

		void OnDuplicateEntity();

		// UI Panels
		void UI_Toolbar();
	private:
		OrthographicCameraController m_CameraController;

		Ref<Framebuffer> m_Framebuffer;

		Ref<Scene> m_ActiveScene;
		Ref<Scene> m_EditorScene;
		std::filesystem::path m_EditorScenePath;
		Entity m_SquareEntity;
		Entity m_CameraEntity;
		Entity m_SecondCamera;
		
		Entity m_HoveredEntity;

		bool m_PrimaryCamera = true;
		EditorCamera m_EditorCamera;

		enum class SceneState
		{
			Edit = 0, Play = 1
		};

		SceneState m_SceneState = SceneState::Edit;

		// ImGui
		glm::vec2 m_ViewportSize = glm::vec2{1.0f};
		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportBounds[2];

		// Gizmos
		int m_GizmoType = -1;
		bool m_IsGizmoInUse = false;

		bool m_ShowPhysicsColliders = true;

		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
		ContentBrowserPanel m_ContentBrowserPanel;

		// Editor Resources
		Ref<Texture2D> m_PlayButtonTexture, m_StopButtonTexture;
	};

}

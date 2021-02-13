#pragma once
#include <Engine.h>
#include "Panels/SceneHierarchyPanel.h"
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

		void NewScene();
		void OpenScene();
		void SaveSceneAs();
		void SaveScene();
	private:
		OrthographicCameraController m_CameraController;

		Ref<Framebuffer> m_Framebuffer;

		Ref<Scene> m_ActiveScene;
		Entity m_SquareEntity;
		Entity m_CameraEntity;
		Entity m_SecondCamera;
		
		Entity m_SelectedEntity;

		bool m_PrimaryCamera = true;

		EditorCamera m_EditorCamera;

		// ImGui
		glm::vec2 m_ViewportSize = glm::vec2{1.0f};
		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportBounds[2];

		// Gizmos
		int m_GizmoType = -1;
		bool m_IsGizmoInUse = false;

		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;

		// Scene Filepath
		std::string m_SceneFilePath;
	};

}

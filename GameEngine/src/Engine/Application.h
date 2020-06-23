#pragma once

#include "Engine/Core.h"

#include "Window.h"
#include "Engine/LayerStack.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"

#include "Engine/ImGui/ImGuiLayer.h"

#include "Renderer/Shader.h"
#include "Renderer/VertexArray.h"
#include "Renderer/OrthographicCamera.h"

namespace Engine
{
	class VertexBuffer;

	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		inline static Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;

		std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<VertexArray> m_VertexArray;
		
		std::shared_ptr<Shader> m_SquareShader;
		std::shared_ptr<VertexArray> m_SquareVA;

		OrthographicCamera m_Camera;
		
		float m_HorizontalMovement = 0.0f;
		float m_VerticalMovement = 0.0f;
		float m_MoveSpeed = 0.025f;
		float m_RotationMovement = 0.0f;
		float m_RotateSpeed = 0.5f;
	private:
		static Application* s_Instance;
	};

	// To be definded in CLIENT
	Application* CreateApplication();
}

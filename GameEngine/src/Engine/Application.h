#pragma once

#include "Engine/Core.h"

#include "Window.h"
#include "Engine/LayerStack.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"

#include "Engine/ImGui/ImGuiLayer.h"

#include "Renderer/Shader.h"

namespace Engine
{
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

		unsigned int m_VertexArray, m_VertexBuffer, m_IndexBuffer;
		std::unique_ptr<Shader> m_Shader;
	private:
		static Application* s_Instance;
	};

	// To be definded in CLIENT
	Application* CreateApplication();
}
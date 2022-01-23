#pragma once

#include "Engine/Core/Base.h"

#include "Engine/Core/Window.h"
#include "Engine/Core/LayerStack.h"
#include "Engine/Events/Event.h"
#include "Engine/Events/ApplicationEvent.h"

#include "Engine/ImGui/ImGuiLayer.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>

int main(int argc, char** argv);

namespace Engine
{
	struct ApplicationCommandLineArgs
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int index) const
		{
			ENGINE_CORE_ASSERT(index < Count, "Index less than Count");
			return Args[index];
		}
	};
	
	class Application
	{
	public:
		Application(const std::string& name = "Game Engine", ApplicationCommandLineArgs args = ApplicationCommandLineArgs());
		virtual ~Application();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		Window& GetWindow() { return *m_Window; }

		void Close();

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }
		
		static Application& Get() { return *s_Instance; }

		ApplicationCommandLineArgs GetCommandLineArgs() const { return m_CommandLineArgs; }
	private:
		void Run();
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		ApplicationCommandLineArgs m_CommandLineArgs;
		Scope<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;

		// Mono App
		MonoDomain* m_MonoDomain;
		MonoAssembly* m_MonoAssembly;
		MonoImage* m_MonoAssemblyImage;

		// Mono Methods
		MonoMethod* m_PtrTickMethod;

		// Mono Objects
		MonoObject* m_PtrGameObject;
		uint32_t m_GameObjectGCHandle = 0;
	private:
		static Application* s_Instance;
		friend int ::main(int argc, char** argv);
	};

	// To be definded in CLIENT
	Application* CreateApplication(ApplicationCommandLineArgs args);
}

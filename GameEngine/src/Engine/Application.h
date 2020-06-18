#pragma once

#include "Core.h"
#include "Window.h"
#include "Events/Event.h"

namespace Engine
{
	class ENGINE_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	private:
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
	};

	// To be definded in CLIENT
	Application* CreateApplication();
}
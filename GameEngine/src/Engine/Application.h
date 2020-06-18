#pragma once

#include "Core.h"

namespace Engine
{
	class ENGINE_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	// To be definded in CLIENT
	Application* CreateApplication();
}
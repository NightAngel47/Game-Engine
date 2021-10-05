#include <Engine.h>
#include <Engine/Core/EntryPoint.h>

#include "ExampleLayer.h"
#include "Sandbox2D.h"

class Sandbox : public Engine::Application
{
public:
	Sandbox(Engine::ApplicationCommandLineArgs args)
		: Engine::Application("Sandbox", args)
	{
		ENGINE_PROFILE_FUNCTION();
		
		//PushLayer(new ExampleLayer());
		PushLayer(new Sandbox2D());
	}

	~Sandbox()
	{
		ENGINE_PROFILE_FUNCTION();
	}
};

Engine::Application* CreateApplication(Engine::ApplicationCommandLineArgs args)
{
	return new Sandbox(args);
}

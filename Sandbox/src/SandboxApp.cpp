#include <Engine.h>
#include <Engine/Core/EntryPoint.h>

//#include "ExampleLayer.h"
//#include "Sandbox2D.h"
#include "StandaloneLayer.h"

class Sandbox : public Engine::Application
{
public:
	Sandbox(const Engine::ApplicationSpecification& specification)
		: Engine::Application(specification)
	{
		ENGINE_PROFILE_FUNCTION();
		ENGINE_CORE_TRACE("Engine Startup - Creating Layers");
		
		//PushLayer(new ExampleLayer());
		//PushLayer(new Sandbox2D());
		PushLayer(new Standalone());
	}

	~Sandbox()
	{
		ENGINE_PROFILE_FUNCTION();
	}
};

Engine::Application* Engine::CreateApplication(Engine::ApplicationCommandLineArgs args)
{
	ENGINE_CORE_TRACE("Engine Startup - Creating App");
	ApplicationSpecification spec;
	spec.Name = "Sandbox";
#if ENGINE_DIST
	spec.WorkingDirectory = "";
#else
	spec.WorkingDirectory = "../Engine-Editor";
#endif
	spec.CommandLineArgs = args;

	return new Sandbox(spec);
}

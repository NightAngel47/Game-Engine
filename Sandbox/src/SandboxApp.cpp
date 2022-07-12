#include <Engine.h>
#include <Engine/Core/EntryPoint.h>

#include "ExampleLayer.h"
#include "Sandbox2D.h"

class Sandbox : public Engine::Application
{
public:
	Sandbox(const Engine::ApplicationSpecification& specification)
		: Engine::Application(specification)
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

Engine::Application* Engine::CreateApplication(Engine::ApplicationCommandLineArgs args)
{
	ApplicationSpecification spec;
	spec.Name = "Sandbox";
	spec.WorkingDirectory = "../Engine-Editor";
	spec.CommandLineArgs = args;

	return new Sandbox(spec);
}

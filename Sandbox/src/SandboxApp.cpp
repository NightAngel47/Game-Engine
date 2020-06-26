#include <Engine.h>
#include <Engine/Core/EntryPoint.h>

#include "ExampleLayer.h"
#include "Sandbox2D.h"

class Sandbox : public Engine::Application
{
public:
	Sandbox()
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

Engine::Application* Engine::CreateApplication()
{
	return new Sandbox();
}
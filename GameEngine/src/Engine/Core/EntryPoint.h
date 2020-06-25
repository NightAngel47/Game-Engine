#pragma once
#include "Engine/Core/Core.h"

#ifdef ENGINE_PLATFORM_WINDOWS

extern Engine::Application* Engine::CreateApplication();

int main(int argc, char** argv)
{
	Engine::Log::Init();
	ENGINE_CORE_WARN("Initialized Log!");
	ENGINE_INFO("Hello!");
	
	auto app = Engine::CreateApplication();
	app->Run();
	delete app;
}

#endif

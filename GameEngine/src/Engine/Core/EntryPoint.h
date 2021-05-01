#pragma once
#include "Engine/Core/Base.h"
#include "Engine/Core/Application.h"
#include "Engine/Debug/Instrumentor.h"

#ifdef ENGINE_PLATFORM_WINDOWS

extern Engine::Application* Engine::CreateApplication(ApplicationCommandLineArgs args);

extern "C" {
inline __declspec(dllexport) uint32_t NvOptimusEnablement = 0x00000001;
inline __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

int main(int argc, char** argv)
{
	Engine::Log::Init();

	ENGINE_PROFILE_BEGIN_SESSION("Startup", "EngineProfile-Startup.json");
	auto app = Engine::CreateApplication({argc, argv});
	ENGINE_PROFILE_END_SESSION();
	
	ENGINE_PROFILE_BEGIN_SESSION("Runtime", "EngineProfile-Runtime.json");
	app->Run();
	ENGINE_PROFILE_END_SESSION();
	
	ENGINE_PROFILE_BEGIN_SESSION("Shutdown", "EngineProfile-Shutdown.json");
	delete app;
	ENGINE_PROFILE_END_SESSION();
}

#endif

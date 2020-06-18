#pragma once

#ifdef GE_PLATFORM_WINDOWS

extern Engine::Application* Engine::CreateApplication();

int main(int argc, char** argv)
{
	Engine::Log::Init();
	ENGINE_CORE_WARN("Initialized Log!");
	int a  = 5;
	ENGINE_INFO("Hello! Var={0}", a);
	
	auto app = Engine::CreateApplication();
	app->Run();
	delete app;
}

#endif

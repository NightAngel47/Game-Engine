#include <enginepch.h>
#include "StandaloneLayer.h"

static Engine::Ref<Engine::Font> s_Font;

void Standalone::OnAttach()
{
	s_Font = Engine::Font::GetDefault();
	std::filesystem::path projectFilePath;

#if ENGINE_DIST
	projectFilePath = "SpaceGame.gameproj";
#else
	auto commandLineArgs = Engine::Application::Get().GetSpecification().CommandLineArgs;
	if (commandLineArgs.Count > 1)
	{
		projectFilePath = commandLineArgs[1];
	}
	else
	{
		ENGINE_CORE_ERROR("Didn't load valid project, closing.");
		Engine::Application::Get().Close();
	}
#endif

	if (Engine::Project::Load(projectFilePath))
	{
		Engine::AssetHandle handle = Engine::Project::GetActive()->GetConfig().RuntimeStartScene;
		Engine::SceneManager::LoadScene(handle);
	}
	else
	{
		ENGINE_CORE_ERROR("Didn't load valid project, closing.");
		Engine::Application::Get().Close();
	}
}

void Standalone::OnDetach()
{
	// TODO find a better place to stop scene? Crashes here
	//m_ActiveScene->OnRuntimeStop();
}

void Standalone::OnUpdate(Engine::Timestep ts)
{
	Engine::RenderCommand::SetClearColor({0.0f, 0.0f, 0.0f, 1.0f});
	Engine::RenderCommand::Clear();

	auto& window = Engine::Application::Get().GetWindow();
	Engine::SceneManager::GetActiveScene()->OnViewportResize(window.GetWidth(), window.GetHeight());

	Engine::SceneManager::GetActiveScene()->OnUpdateRuntime(ts);

	glm::vec2 mousePos = Engine::Input::GetMousePosition();
	Engine::UIEngine::SetViewportMousePos(mousePos.x, window.GetHeight() - mousePos.y); // y is inverted
}

void Standalone::OnImGuiRender()
{
}

void Standalone::OnEvent(Engine::Event& e)
{
}

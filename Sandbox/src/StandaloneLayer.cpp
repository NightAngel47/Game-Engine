#include <enginepch.h>
#include "StandaloneLayer.h"

static Engine::Ref<Engine::Font> s_Font;

void Standalone::OnAttach()
{
	s_Font = Engine::Font::GetDefault();
	
	m_ActiveScene = Engine::CreateRef<Engine::Scene>();

	bool projectLoaded = true;
	auto commandLineArgs = Engine::Application::Get().GetSpecification().CommandLineArgs;
	if (commandLineArgs.Count > 1)
	{
		auto projectFilePath = commandLineArgs[1];
		if (Engine::Project::Load(projectFilePath))
		{
			Engine::ScriptEngine::Init(); 
			std::filesystem::path startScenePath = Engine::Project::GetActive()->GetConfig().StartScene;

			m_ActiveScene = Engine::CreateRef<Engine::Scene>(startScenePath.filename().string());
			auto& window = Engine::Application::Get().GetWindow();
			m_ActiveScene->OnViewportResize(window.GetWidth(), window.GetHeight());

			Engine::SceneSerializer serializer(m_ActiveScene);
			serializer.Deserialize(startScenePath);
		}
		else
		{
			projectLoaded = false;
		}
	}
	else
	{
		projectLoaded = false;
	}

	if (!projectLoaded)
	{
		ENGINE_CORE_ERROR("Didn't load valid project, closing.");
		Engine::Application::Get().Close();
	}

	m_ActiveScene->OnRuntimeStart();
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
	m_ActiveScene->OnViewportResize(window.GetWidth(), window.GetHeight());

	m_ActiveScene->OnUpdateRuntime(ts);

	glm::vec2 mousePos = Engine::Input::GetMousePosition();
	Engine::UIEngine::SetViewportMousePos(mousePos.x, window.GetHeight() - mousePos.y); // y is inverted
}

void Standalone::OnImGuiRender()
{
}

void Standalone::OnEvent(Engine::Event& e)
{
}

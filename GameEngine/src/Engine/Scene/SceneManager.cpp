#include "enginepch.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Project/Project.h"
#include "Engine/Core/Application.h"

namespace Engine
{
	Ref<Scene> SceneManager::LoadScene(const AssetHandle handle)
	{
		return Project::GetActive()->GetSceneManager()->LoadScene(handle);
	}

	Ref<Scene> SceneManager::CreateNewScene(const std::string& name)
	{
		return Project::GetActive()->GetSceneManager()->CreateNewScene(name);
	}

	const Ref<Scene> SceneManager::GetActiveScene()
	{
		return Project::GetActive()->GetSceneManager()->GetActiveScene();
	}

	const std::map<Engine::AssetHandle, std::string>& SceneManager::GetSceneMap()
	{
		return Project::GetActive()->GetSceneManager()->GetSceneMap();
	}
}

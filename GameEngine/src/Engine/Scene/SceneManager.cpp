#include "enginepch.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Project/Project.h"

namespace Engine
{
	Ref<Scene> SceneManager::LoadScene(const AssetHandle handle)
	{
		return Project::GetActive()->GetSceneManager()->LoadScene(handle);
	}

	Ref<Scene> SceneManager::LoadScene(const std::filesystem::path& path)
	{
		return Project::GetActive()->GetSceneManager()->LoadScene(path);
	}

	const Ref<Scene> SceneManager::GetActiveScene()
	{
		return Project::GetActive()->GetSceneManager()->GetActiveScene();
	}

	Ref<Scene> SceneManager::CreateNewScene(const std::string& name)
	{
		return Project::GetActive()->GetSceneManager()->CreateNewScene(name);
	}
}

#include "enginepch.h"
#include "Engine/Scene/EditorSceneManager.h"
#include "Engine/Asset/AssetManager.h"

namespace Engine
{
	Ref<Scene> EditorSceneManager::LoadScene(const AssetHandle handle)
	{
		m_ActiveScene = AssetManager::GetAsset<Scene>(handle);
		return m_ActiveScene;
	}

	Ref<Scene> EditorSceneManager::LoadScene(const std::filesystem::path& path)
	{
		const AssetHandle handle = Project::GetActive()->GetEditorAssetManager()->GetAssetHandleFromFilePath(path);
		m_ActiveScene = AssetManager::GetAsset<Scene>(handle);
		return m_ActiveScene;
	}

	Ref<Scene> EditorSceneManager::CreateNewScene(const std::string& name)
	{
		m_ActiveScene = CreateRef<Scene>(name);
		return m_ActiveScene;
	}

	void EditorSceneManager::LoadSceneCopy(const Ref<Scene> scene)
	{
		m_ActiveScene = Scene::Copy(scene);
	}
}

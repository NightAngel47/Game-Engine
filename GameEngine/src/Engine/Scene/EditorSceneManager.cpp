#include "enginepch.h"
#include "Engine/Scene/EditorSceneManager.h"
#include "Engine/Asset/AssetManager.h"

namespace Engine
{
	EditorSceneManager::EditorSceneManager()
	{
		const auto& registry = Project::GetActive()->GetEditorAssetManager()->GetAssetRegistry();
		for (const auto& [handle, metadata] : registry)
		{
			if (metadata.Type == AssetType::Scene)
				m_SceneMap[handle] = metadata.Path.generic_string();
		}
	}

	Ref<Scene> EditorSceneManager::LoadScene(const AssetHandle handle)
	{
		ActiveSceneRuntimeStop();

		auto scene = AssetManager::GetAsset<Scene>(handle);
		m_ActiveScene = Scene::Copy(scene);
		m_ActiveScene->SetSceneName(scene->GetSceneName());
		m_ActiveScene->Handle = handle;

		ActiveSceneRuntimeStart();

		return m_ActiveScene;
	}

	Ref<Scene> EditorSceneManager::LoadScene(const std::filesystem::path& path)
	{
		ActiveSceneRuntimeStop();

		const AssetHandle handle = Project::GetActive()->GetEditorAssetManager()->GetAssetHandleFromFilePath(path);
		auto scene = AssetManager::GetAsset<Scene>(handle);
		m_ActiveScene = Scene::Copy(scene);
		m_ActiveScene->SetSceneName(scene->GetSceneName());
		m_ActiveScene->Handle = handle;

		ActiveSceneRuntimeStart();

		return m_ActiveScene;
	}

	Ref<Scene> EditorSceneManager::CreateNewScene(const std::string& name)
	{
		ActiveSceneRuntimeStop();

		m_ActiveScene = CreateRef<Scene>(name);

		ActiveSceneRuntimeStart();

		return m_ActiveScene;
	}

	Ref<Scene> EditorSceneManager::LoadSceneCopy(const Ref<Scene> scene)
	{
		return m_ActiveScene = Scene::Copy(scene);
	}

}

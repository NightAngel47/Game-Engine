#include "enginepch.h"
#include "Engine/Scene/RuntimeSceneManager.h"
#include "Engine/Asset/AssetManager.h"

namespace Engine
{

	RuntimeSceneManager::RuntimeSceneManager()
	{
		const auto& pack = Project::GetActive()->GetRuntimeAssetManager()->GetAssets();
		for (const auto& [handle, metadata] : pack)
		{
			if (metadata.Type == AssetType::Scene)
				m_SceneMap[handle] = metadata.Path.generic_string();
		}
	}

	Ref<Scene> RuntimeSceneManager::LoadScene(const AssetHandle handle)
	{
		if (m_ActiveScene->Handle.IsValid() && m_ActiveScene->IsRunning())
			m_ActiveScene->OnRuntimeStop();

		auto scene = AssetManager::GetAsset<Scene>(handle);
		m_ActiveScene = Scene::Copy(scene);
		m_ActiveScene->SetSceneName(scene->GetSceneName());
		m_ActiveScene->Handle = handle;

		m_ActiveScene->OnRuntimeStart();

		return m_ActiveScene;
	}

	Ref<Scene> RuntimeSceneManager::CreateNewScene(const std::string& name)
	{
		if (m_ActiveScene->Handle.IsValid() && m_ActiveScene->IsRunning())
			m_ActiveScene->OnRuntimeStop();

		m_ActiveScene = CreateRef<Scene>(name);

		m_ActiveScene->OnRuntimeStart();

		return m_ActiveScene;
	}

}

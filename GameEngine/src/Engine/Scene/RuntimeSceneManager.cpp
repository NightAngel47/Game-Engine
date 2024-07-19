#include "enginepch.h"
#include "Engine/Scene/RuntimeSceneManager.h"
#include "Engine/Asset/AssetManager.h"

namespace Engine
{

	RuntimeSceneManager::RuntimeSceneManager()
	{
		for (const auto& [handle, asset] : AssetManager::GetAssetsOfType(AssetType::Scene))
		{
			if (asset.get() == nullptr || !asset->Handle.IsValid())
			{
				ENGINE_CORE_WARN("Scene Asset: {}, Invalid!", handle);
				continue;
			}

			m_SceneMap[handle] = As<Scene>(asset)->GetSceneName();
		}
	}

	Ref<Scene> RuntimeSceneManager::LoadScene(const AssetHandle handle)
	{
		ENGINE_CORE_TRACE("Loading Scene: {}", handle);
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

#pragma once
#include "Engine/Scene/Scene.h"

namespace Engine
{
	class SceneManager
	{
	public:
		static Ref<Scene> SceneManager::LoadScene(const AssetHandle handle);
		static Ref<Scene> SceneManager::LoadScene(const std::filesystem::path& path);

		static Ref<Scene> SceneManager::CreateNewScene(const std::string& name = "Untitled");

		static const Ref<Scene> SceneManager::GetActiveScene();
	};
}

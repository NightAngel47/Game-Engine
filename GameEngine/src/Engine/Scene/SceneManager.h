#pragma once
#include "Engine/Scene/Scene.h"

namespace Engine
{
	class SceneManager
	{
	public:
		static Ref<Scene> LoadScene(const AssetHandle handle);

		static Ref<Scene> CreateNewScene(const std::string& name = "Untitled");

		static const Ref<Scene> GetActiveScene();

		static const std::map<AssetHandle, std::string>& GetSceneMap();
	};
}

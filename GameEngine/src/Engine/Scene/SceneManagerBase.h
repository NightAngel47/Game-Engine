#pragma once
#include "Engine/Scene/Scene.h"

namespace Engine
{
	class SceneManagerBase
	{
	public:
		virtual Ref<Scene> LoadScene(const AssetHandle handle) = 0;

		virtual Ref<Scene> CreateNewScene(const std::string& name = "Untitled") = 0;

		virtual const Ref<Scene> GetActiveScene() const = 0;

		virtual const std::map<AssetHandle, std::string>& GetSceneMap() const = 0;
	};
}

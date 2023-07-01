#pragma once
#include "Engine/Scene/Scene.h"

namespace Engine
{
	class SceneManager
	{
	public:
		static Ref<Scene> LoadScene(const AssetHandle handle);
		static Ref<Scene> LoadScene(const std::filesystem::path& path);

		static Ref<Scene> CreateNewScene(const std::string& name = "Untitled");

		static const Ref<Scene> GetActiveScene();
	};
}

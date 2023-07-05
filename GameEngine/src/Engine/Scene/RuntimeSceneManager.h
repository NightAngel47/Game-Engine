#pragma once
#include "Engine/Scene/SceneManagerBase.h"

namespace Engine
{
	class RuntimeSceneManager : public SceneManagerBase
	{
	public:
		RuntimeSceneManager();

		Ref<Scene> LoadScene(const AssetHandle handle) override;

		Ref<Scene> CreateNewScene(const std::string& name = "Untitled") override;

		const Ref<Scene> GetActiveScene() const override { return m_ActiveScene; }

		const std::map<AssetHandle, std::string>& GetSceneMap() const override { return m_SceneMap; }

	private:
		Ref<Scene> m_ActiveScene = CreateRef<Scene>("Untitled");
		std::map<AssetHandle, std::string> m_SceneMap;
	};
}

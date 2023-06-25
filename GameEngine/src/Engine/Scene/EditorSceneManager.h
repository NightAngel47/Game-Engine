#pragma once
#include "Engine/Scene/SceneManagerBase.h"
#include "Engine/Scene/Scene.h"

namespace Engine
{
	class EditorSceneManager : public SceneManagerBase
	{
	public:
		Ref<Scene> LoadScene(const AssetHandle handle) override;
		Ref<Scene> LoadScene(const std::filesystem::path& path) override;

		Ref<Scene> CreateNewScene(const std::string& name = "Untitled") override;

		const Ref<Scene> GetActiveScene() const override { return m_ActiveScene; }

		void LoadSceneCopy(const Ref<Scene> scene);
	private:
		Ref<Scene> m_ActiveScene;
	};
}

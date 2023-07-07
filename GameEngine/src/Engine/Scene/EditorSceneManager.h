#pragma once
#include "Engine/Scene/SceneManagerBase.h"
#include "Engine/Scene/Scene.h"

namespace Engine
{
	enum class EditorSceneState
	{
		Edit = 0, Play = 1, Simulate = 2
	};

	class EditorSceneManager : public SceneManagerBase
	{
	public:
		EditorSceneManager();

		Ref<Scene> LoadScene(const AssetHandle handle) override;
		Ref<Scene> LoadScene(const std::filesystem::path& path);

		Ref<Scene> CreateNewScene(const std::string& name = "Untitled") override;

		const Ref<Scene> GetActiveScene() const override { return m_ActiveScene; }

		Ref<Scene> LoadSceneCopy(const Ref<Scene> scene);

		const EditorSceneState GetEditorSceneState() const { return m_SceneState; }
		void SetEditorSceneState(const EditorSceneState state) { m_SceneState = state; }

		const std::map<AssetHandle, std::string>& GetSceneMap() const override { return m_SceneMap; }

	private:
		void ActiveSceneRuntimeStop()
		{
			if (m_SceneState == EditorSceneState::Play)
				m_ActiveScene->OnRuntimeStop();
		}

		void ActiveSceneRuntimeStart()
		{
			if (m_SceneState == EditorSceneState::Play)
				m_ActiveScene->OnRuntimeStart();
		}

	private:
		Ref<Scene> m_ActiveScene = CreateRef<Scene>("Untitled");
		std::map<AssetHandle, std::string> m_SceneMap;

		EditorSceneState m_SceneState = EditorSceneState::Edit;
	};
}

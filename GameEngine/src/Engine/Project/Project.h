#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Asset/EditorAssetManager.h"
#include "Engine/Asset/RuntimeAssetManager.h"
#include "Engine/Scene/EditorSceneManager.h"
#include "Engine/Scene/RuntimeSceneManager.h"

#include <string>
#include <filesystem>

namespace Engine
{
	struct ProjectConfig
	{
		std::string Name;

		AssetHandle EditorStartScene;
		AssetHandle RuntimeStartScene;

		std::filesystem::path AssetDirectory;
		std::filesystem::path AssetRegistryPath;
		std::filesystem::path ScriptModulePath;
	};

	class Project
	{
	public:
		ProjectConfig& GetConfig() { return m_Config; }

		static Ref<Project> GetActive() 
		{
			ENGINE_CORE_ASSERT(s_ActiveProject, "No active project!");
			return s_ActiveProject;
		}

		static const std::filesystem::path GetActiveProjectDirectory()
		{
			ENGINE_CORE_ASSERT(s_ActiveProject, "No active project!");
			return s_ActiveProject->GetProjectDirectory();
		}

		static std::filesystem::path GetActiveAssetDirectory()
		{
			ENGINE_CORE_ASSERT(s_ActiveProject, "No active project!");
			return s_ActiveProject->GetAssetDirectory();
		}

		static std::filesystem::path GetActiveAssetRegistryPath()
		{
			ENGINE_CORE_ASSERT(s_ActiveProject, "No active project!");
			return s_ActiveProject->GetAssetRegistryPath();
		}

		static std::filesystem::path GetActiveAssetFileSystemPath(const std::filesystem::path& path)
		{
			ENGINE_CORE_ASSERT(s_ActiveProject, "No active project!");
			return s_ActiveProject->GetAssetFileSystemPath(path);
		}

		const std::filesystem::path GetProjectDirectory() { return m_ProjectDirectory; }
		std::filesystem::path GetAssetDirectory() { return (GetProjectDirectory() / m_Config.AssetDirectory); }
		std::filesystem::path GetAssetRegistryPath() { return (GetAssetDirectory() / m_Config.AssetRegistryPath); }
		std::filesystem::path GetAssetFileSystemPath(const std::filesystem::path& path) { return (GetAssetDirectory() / path); }

		Ref<AssetManagerBase> GetAssetManager() { return m_AssetManager; }
		Ref<EditorAssetManager> GetEditorAssetManager() { return As<EditorAssetManager>(m_AssetManager); }
		Ref<RuntimeAssetManager> GetRuntimeAssetManager() { return As<RuntimeAssetManager>(m_AssetManager); }

		Ref<SceneManagerBase> GetSceneManager() { return m_SceneManager; }
		Ref<EditorSceneManager> GetEditorSceneManager() { return As<EditorSceneManager>(m_SceneManager); }
		Ref<RuntimeSceneManager> GetRuntimeSceneManager() { return As<RuntimeSceneManager>(m_SceneManager); }

		static Ref<Project> New();
		static Ref<Project> Load(const std::filesystem::path& path);
		static void SaveAs(const std::filesystem::path& path);
		static void Save();

	private:
		ProjectConfig m_Config;
		std::filesystem::path m_ProjectDirectory;
		Ref<AssetManagerBase> m_AssetManager;
		Ref<SceneManagerBase> m_SceneManager;
			
		inline static Ref<Project> s_ActiveProject;
	};
}

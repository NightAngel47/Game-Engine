#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/EditorAssetManager.h"
#include "Engine/Asset/RuntimeAssetManager.h"
#include "Engine/Scene/SceneManager.h"
#include "Engine/Scene/EditorSceneManager.h"
#include "Engine/Scene/RuntimeSceneManager.h"

#include <string>
#include <filesystem>

namespace Engine
{
	struct ProjectConfig
	{
		std::string Name;

		std::filesystem::path StartScene;

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

		static const std::filesystem::path GetProjectDirectory()
		{
			ENGINE_CORE_ASSERT(s_ActiveProject, "No active project!");
			return s_ActiveProject->m_ProjectDirectory;
		}

		static std::filesystem::path GetAssetDirectory()
		{
			ENGINE_CORE_ASSERT(s_ActiveProject, "No active project!");
			return GetProjectDirectory() / s_ActiveProject->m_Config.AssetDirectory;
		}

		static std::filesystem::path GetAssetRegistryPath()
		{
			ENGINE_CORE_ASSERT(s_ActiveProject, "No active project!");
			return GetAssetDirectory() / s_ActiveProject->m_Config.AssetRegistryPath;
		}

		static std::filesystem::path GetAssetFileSystemPath(const std::filesystem::path& path)
		{
			ENGINE_CORE_ASSERT(s_ActiveProject, "No active project!");
			return GetAssetDirectory() / path;
		}

		Ref<AssetManagerBase> GetAssetManager() { return m_AssetManager; }
		Ref<EditorAssetManager> GetEditorAssetManager() { return As<EditorAssetManager>(m_AssetManager); }
		Ref<RuntimeAssetManager> GetRuntimeAssetManager() { return As<RuntimeAssetManager>(m_AssetManager); }

		Ref<SceneManagerBase> GetSceneManager() { return m_SceneManager; }
		Ref<EditorSceneManager> GetEditorSceneManager() { return As<EditorSceneManager>(m_SceneManager); }
		Ref<RuntimeSceneManager> GetRuntimeSceneManager() { return As<RuntimeSceneManager>(m_SceneManager); }

		static Ref<Project> New();
		static Ref<Project> Load(const std::filesystem::path& path);
		static void Save(const std::filesystem::path& path);

	private:
		ProjectConfig m_Config;
		std::filesystem::path m_ProjectDirectory;
		Ref<AssetManagerBase> m_AssetManager;
		Ref<SceneManagerBase> m_SceneManager;
			
		inline static Ref<Project> s_ActiveProject;
	};
}

#pragma once

#include "Engine/Core/Base.h"

#include <string>
#include <filesystem>

namespace Engine
{
	struct ProjectConfig
	{
		std::string Name;

		std::filesystem::path StartScene;

		std::filesystem::path AssetDirectory;
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

		static const std::filesystem::path& GetProjectDirectory()
		{
			ENGINE_CORE_ASSERT(s_ActiveProject, "No active project!");
			return s_ActiveProject->m_ProjectDirectory;
		}

		static std::filesystem::path GetAssetDirectory()
		{
			ENGINE_CORE_ASSERT(s_ActiveProject, "No active project!");
			return GetProjectDirectory() / s_ActiveProject->m_Config.AssetDirectory;
		}

		static std::filesystem::path GetAssetFileSystemPath(const std::filesystem::path& path)
		{
			ENGINE_CORE_ASSERT(s_ActiveProject, "No active project!");
			return GetAssetDirectory() / path;
		}

		static Ref<Project> New();
		static Ref<Project> Load(const std::filesystem::path& path);
		static void Save(const std::filesystem::path& path);

	private:
		ProjectConfig m_Config;
		std::filesystem::path m_ProjectDirectory;

		inline static Ref<Project> s_ActiveProject;
	};
}

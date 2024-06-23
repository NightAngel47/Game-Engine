#include "enginepch.h"
#include "Engine/Project/Project.h"
#include "Engine/Project/ProjectSerializer.h"
#include "Engine/Core/Application.h"
#include "Engine/Scripting/ScriptEngine.h"
#include "Engine/Audio/AudioEngine.h"

namespace Engine
{
	Ref<Project> Project::New()
	{
		s_ActiveProject = CreateRef<Project>();
		return s_ActiveProject;
	}

	Ref<Project> Project::Load(const std::filesystem::path& path)
	{
		Ref<Project> project = CreateRef<Project>();
		ProjectSerializer serializer(project);
		if (serializer.Deserialize(path))
		{
			project->m_ProjectDirectory = path.parent_path();
			s_ActiveProject = project;

			ScriptEngine::Init();

			if (Application::Get().GetSpecification().Runtime)
			{
				project->m_AssetManager = CreateRef<RuntimeAssetManager>();
				project->m_SceneManager = CreateRef<RuntimeSceneManager>();
			}
			else
			{
				project->m_AssetManager = CreateRef<EditorAssetManager>();
				project->m_SceneManager = CreateRef<EditorSceneManager>();
			}
			
			AudioEngine::Init();

			return s_ActiveProject;
		}

		return nullptr;
	}

	void Project::SaveAs(const std::filesystem::path& path)
	{
		ProjectSerializer serializer(s_ActiveProject);
		s_ActiveProject->m_ProjectDirectory = path.parent_path();
		serializer.Serialize(path);
	}

	void Project::Save()
	{
		ProjectSerializer serializer(s_ActiveProject);
		auto project = Project::GetActive();

		std::filesystem::path projectFile{project->GetConfig().Name + ".gameproj"};
		std::filesystem::path path{project->GetActiveProjectDirectory() / projectFile};
		serializer.Serialize(path);
	}

}

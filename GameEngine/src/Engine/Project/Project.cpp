#include "enginepch.h"
#include "Engine/Project/Project.h"
#include "Engine/Project/ProjectSerializer.h"
#include "Engine/Scripting/ScriptEngine.h"

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

			project->m_AssetManager = CreateRef<EditorAssetManager>();
			
			AssetImporter::Init();

			ScriptEngine::Init();

			return s_ActiveProject;
		}

		return nullptr;
	}

	void Project::Save(const std::filesystem::path& path)
	{
		ProjectSerializer serializer(s_ActiveProject);
		s_ActiveProject->m_ProjectDirectory = path.parent_path();
		serializer.Serialize(path);
	}
}

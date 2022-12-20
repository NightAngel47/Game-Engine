#pragma once
#include "Engine/Project/Project.h"

#include <filesystem>

namespace Engine
{
	class ProjectSerializer
	{
	public:
		ProjectSerializer(Ref<Project> project)
			: m_Project(project) {}

		void Serialize(const std::filesystem::path& filepath);

		bool Deserialize(const std::filesystem::path& filepath);

	private:
		Ref<Project> m_Project;
	};
}

#pragma once
#include "Engine/Scene/Entity.h"

namespace Engine
{
	class PrefabSerializer
	{
	public:
		PrefabSerializer(Entity entity, const Ref<Scene>& scene)
			:m_Entity(entity), m_Scene(scene) {}

		void Serialize(const std::filesystem::path& filepath);

		Entity Deserialize(const std::filesystem::path& filepath);
	private:
		Entity m_Entity;
		Ref<Scene> m_Scene;
	};
}

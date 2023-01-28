#pragma once
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Entity.h"

#include <yaml-cpp/yaml.h>

namespace Engine
{
	class EntitySerializer
	{
	public:
		EntitySerializer(Entity entity, const Ref<Scene>& scene)
			: m_Entity(entity), m_Scene(scene) {}

		void Serialize(YAML::Emitter& out);

		Entity Deserialize(YAML::Node& entity);
	private:
		Entity m_Entity;
		Ref<Scene> m_Scene;
	};
}

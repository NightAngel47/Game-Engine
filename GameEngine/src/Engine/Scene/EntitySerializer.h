#pragma once
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Entity.h"

#include <yaml-cpp/yaml.h>

namespace Engine
{
	class EntitySerializer
	{
	public:
		EntitySerializer() = default;

		void Serialize(YAML::Emitter& out, Entity entity, const Ref<Scene>& scene);

		Entity Deserialize(YAML::Node& entityOut, Entity entity, Ref<Scene>& scene, bool isPrefab = false);
	};
}

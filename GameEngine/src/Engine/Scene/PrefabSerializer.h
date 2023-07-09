#pragma once
#include "Engine/Scene/Entity.h"

namespace Engine
{
	class PrefabSerializer
	{
	public:
		PrefabSerializer() = default;

		void Serialize(const std::filesystem::path& filepath, Entity entity, const Ref<Scene>& scene);

		void Deserialize(const std::filesystem::path& filepath, Entity entity, Ref<Scene>& scene);
	};
}

#pragma once
#include "Engine/Asset/Assets.h"
#include "Engine/Scene/Entity.h"

namespace Engine
{
	class Prefab : public Asset
	{
	public:
		Prefab() = default;
		Prefab(Entity entity, Ref<Scene> originalScene)
			:m_PrefabEntity(entity) 
		{
			m_PrefabScene = CreateRef<Scene>("Prefab Scene");
			m_PrefabScene->CopyEntityFromOtherScene(entity, originalScene);
		}
		~Prefab() = default;

		static AssetType GetStaticType() { return AssetType::Prefab; }
		virtual AssetType GetAssetType() const override { return GetStaticType(); }
	private:
		Entity m_PrefabEntity;
		Ref<Scene> m_PrefabScene;

		friend class PrefabSerializer;
		friend class Scene;
	};

}

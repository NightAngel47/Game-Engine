#include "enginepch.h"
#include "Engine/Scene/Scene.h"

#include "Engine/Scene/Components.h"
#include "Engine/Renderer/Renderer2D.h"

namespace Engine
{
	Scene::Scene()
	{
		
	}

	Scene::~Scene()
	{
		
	}

	entt::entity Scene::CreateEntity()
	{
		return m_Registry.create();
	}

	void Scene::OnUpdate(Timestep ts)
	{
		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group)
		{
			auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
			
			Renderer2D::DrawQuad(transform, sprite.Color);
		}
	}
}

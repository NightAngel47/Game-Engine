#include "enginepch.h"
#include "Engine/Scene/Scene.h"

#include <glm/glm.hpp>

namespace Engine
{
	static void DoMath(const glm::mat4& transform)
	{
		
	}

	static void OnTransfromContruct(entt::registry& registry, entt::entity entity)
	{
		
	}
	
	Scene::Scene()
	{
		struct MeshComponent {};
		struct TransformComponent
		{
			glm::mat4 Transform;

			TransformComponent() = default;
			TransformComponent(const TransformComponent&) = default;
			TransformComponent(const glm::mat4& transform)
				: Transform(transform) {}

			operator glm::mat4&() { return Transform; }
			operator const glm::mat4&() const { return Transform; }
		};
		
		entt::entity myEntity = m_Registry.create();
		m_Registry.emplace<TransformComponent>(myEntity, glm::mat4(1.0f));

		m_Registry.on_construct<TransformComponent>().connect<&OnTransfromContruct>();

		if (m_Registry.has<TransformComponent>(myEntity))
			TransformComponent& transform = m_Registry.get<TransformComponent>(myEntity);

		auto view = m_Registry.view<TransformComponent>();
		for (auto entity : view)
		{
			TransformComponent& transform = view.get<TransformComponent>(entity);
		}

		auto group = m_Registry.group<TransformComponent>(entt::get<MeshComponent>);
		for (auto entity : group)
		{
			auto&[transform, mesh] = group.get<TransformComponent, MeshComponent>(entity);
			
		}
	}

	Scene::~Scene()
	{
		
	}
}

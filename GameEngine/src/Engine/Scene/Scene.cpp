#include "enginepch.h"
#include "Engine/Scene/Scene.h"

#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Components.h"
#include "Engine/Renderer/Renderer2D.h"

#include <glm/glm.hpp>

namespace Engine
{
	Scene::Scene() 
	{
	}

	Scene::Scene(std::string name)
		:m_Name(name) 
	{
	}

	Scene::~Scene() 
	{
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;

		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::OnScenePlay()
	{
		// Create Physics Objects
		{
			m_World = new b2World(m_Gravity);

			m_Registry.view<RigidbodyComponent>().each([=](auto entity, auto& rbc)
			{
				rbc.Rigidbody.CreateBody(*m_World);
				Entity gameEntity = Entity{ entity, this };
				if (gameEntity.HasComponent<BoxColliderComponent>())
					rbc.Rigidbody.CreateFixture(gameEntity.GetComponent<BoxColliderComponent>().BoxCollider.GetShape());
			});
		}

		// Start Scripts
		{
			m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
			{
				if (!nsc.Instance)
				{
					nsc.Instance = nsc.InstantiateScript();
					nsc.Instance->m_Entity = Entity{ entity, this };
					nsc.Instance->OnCreate();
				}
			});
		}
	}

	void Scene::OnSceneStop()
	{
		// Destroy Physics Objects
		{
			m_Registry.view<RigidbodyComponent>().each([=](auto entity, auto& rbc)
			{
				rbc.Rigidbody.DestroyBody(*m_World);
			});

			delete(m_World);
		}
	}

	void Scene::OnUpdateRuntime(Timestep ts)
	{
		// Update scripts
		{
			m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
			{
				if (!nsc.Instance)
				{
					nsc.Instance = nsc.InstantiateScript();
					nsc.Instance->m_Entity = Entity{ entity, this };
					nsc.Instance->OnCreate();
				}
				
				nsc.Instance->OnUpdate(ts);
			});
		}
		
		// Render 2D
		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);
				
				if (camera.Primary)
				{
					mainCamera = &camera.Camera;
					cameraTransform = transform.GetTransform();
					break;
				}
			}
		}

		if (mainCamera)
		{
			Renderer2D::BeginScene(*mainCamera, cameraTransform);
			
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
				
				Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
			}

			Renderer2D::EndScene();
		}
	}

	void Scene::OnUpdatePhysics(Timestep ts)
	{
		m_World->Step(ts, m_VelocityIteractions, m_PositionIteractions);

		auto view = m_Registry.view<TransformComponent, RigidbodyComponent>();
		for (auto entity : view)
		{
			auto [transform, rigidbody] = view.get<TransformComponent, RigidbodyComponent>(entity);

			if (rigidbody.Rigidbody.GetBodyType() != Engine::PhysicsBodyType::Static)
			{
				transform.Position = glm::vec3(rigidbody.Rigidbody.GetPosition(), transform.Position.z);
				transform.Rotation = glm::vec3(transform.Rotation.x, transform.Rotation.y, rigidbody.Rigidbody.GetAngle());
			}
		}
	}

	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		Renderer2D::BeginScene(camera);
			
		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group)
		{
			auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
			
			Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
		}

		Renderer2D::EndScene();
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		// Resize our non-FixedAspectRatio cameras
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
				cameraComponent.Camera.SetViewportSize(width, height);
		}
	}

	Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for(auto entity : view)
		{
			const auto& cameraComponent = view.get<CameraComponent>(entity);
			if(cameraComponent.Primary)
				return Entity{entity, this};
		}

		return {};
	}

	template <typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		static_assert(false);
	}
	
	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{
		
	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{
		
	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		if(m_ViewportWidth > 0 && m_ViewportHeight > 0)
			component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}

	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{
		
	}

	template<>
	void Scene::OnComponentAdded<RigidbodyComponent>(Entity entity, RigidbodyComponent& component)
	{
		Engine::TransformComponent transformComponent = entity.GetComponent<TransformComponent>();
		component.Rigidbody.SetPosition((glm::vec2)transformComponent.Position);
		component.Rigidbody.SetAngle(transformComponent.Rotation.z);
	}

	template<>
	void Scene::OnComponentAdded<BoxColliderComponent>(Entity entity, BoxColliderComponent& component)
	{
		Engine::TransformComponent transformComponent = entity.GetComponent<TransformComponent>();
		component.BoxCollider.SetExtents(transformComponent.Scale.x / 2, transformComponent.Scale.y / 2);
	}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{
		
	}
}

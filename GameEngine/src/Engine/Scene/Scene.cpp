#include "enginepch.h"
#include "Engine/Scene/Scene.h"

#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/ScriptableEntity.h"
#include "Engine/Renderer/Renderer2D.h"

#include "Engine/Scripting/ScriptEngine.h"

#include <glm/glm.hpp>

// box2d
#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include "box2d/b2_circle_shape.h"
#include <box2d/b2_types.h>

namespace Engine
{
	static b2BodyType Rigidbody2DTypeToBox2DBodyType(Rigidbody2DComponent::BodyType bodyType)
	{
		switch (bodyType)
		{
			case Rigidbody2DComponent::BodyType::Static:	return b2BodyType::b2_staticBody;
			case Rigidbody2DComponent::BodyType::Dynamic:	return b2BodyType::b2_dynamicBody;
			case Rigidbody2DComponent::BodyType::Kinematic:	return b2BodyType::b2_kinematicBody;
		}

		ENGINE_ASSERT(false, "Unknown body type!");
		return b2_staticBody;
	}

	template<typename... Component>
	static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		([&]()
		{
			auto view = src.view<Component>();
			for (auto srcEntity : view)
			{
				entt::entity dstEntity = enttMap.at(src.get<IDComponent>(srcEntity).ID);

				auto& srcComponent = src.get<Component>(srcEntity);
				dst.emplace_or_replace<Component>(dstEntity, srcComponent);
			}
		}(), ...);
	}

	template<typename... Component>
	static void CopyComponent(ComponentGroup<Component...>, entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
	{
		CopyComponent<Component...>(dst, src, enttMap);
	}

	template<typename... Component>
	static void CopyComponentIfExists(Entity dst, Entity src)
	{
		([&]()
		{
			if (src.HasComponent<Component>())
				dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
		}(), ...);
	}

	template<typename... Component>
	static void CopyComponentIfExists(ComponentGroup<Component...>, Entity dst, Entity src)
	{
		CopyComponentIfExists<Component...>(dst, src);
	}

	Ref<Scene> Scene::Copy(Ref<Scene> other)
	{
		Ref<Scene> newScene = CreateRef<Scene>();

		newScene->m_ViewportWidth = other->m_ViewportWidth;
		newScene->m_ViewportHeight = other->m_ViewportHeight;

		auto& srcSceneRegistry = other->m_Registry;
		auto& dstceneRegistry = newScene->m_Registry;

		// Create entities in new scene
		auto idView = srcSceneRegistry.view<IDComponent>();
		for (auto e : idView)
		{
			UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
			const auto& name = srcSceneRegistry.get<TagComponent>(e).Tag;
			Entity newEntity = newScene->CreateEntityWithUUID(uuid, name);
		}

		// Copy Components (Except ID and Tag Components)
		CopyComponent(AllComponents{}, dstceneRegistry, srcSceneRegistry, newScene->m_EntityMap);

		return newScene;
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntityWithUUID(UUID(), name);
	}

	Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };

		entity.AddComponent<IDComponent>(uuid);
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;

		m_EntityMap[uuid] = entity;

		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
		m_EntityMap.erase(entity.GetUUID());
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
		for (auto entity : view)
		{
			const auto& cameraComponent = view.get<CameraComponent>(entity);
			if (cameraComponent.Primary)
				return Entity{ entity, this };
		}

		return {};
	}

	void Scene::Step(int frames)
	{
		m_StepFrames = frames;
	}

	void Scene::DuplicateEntity(Entity entity)
	{
		Entity newEntity = CreateEntity(entity.GetName());
		CopyComponentIfExists(AllComponents{}, newEntity, entity);
	}

	Entity Scene::GetEntityWithUUID(UUID uuid)
	{
		ENGINE_CORE_ASSERT(m_EntityMap.find(uuid) != m_EntityMap.end(), "Could not find Entity with UUID: " + std::to_string(uuid) + " in Scene: " + m_Name);
		return { m_EntityMap.at(uuid), this };
	}

	Entity Scene::FindEntityByName(const std::string_view& entityName)
	{
		auto view = m_Registry.view<TagComponent>();
		for (auto entity : view)
		{
			const TagComponent& tc = view.get<TagComponent>(entity);
			if (tc.Tag == entityName)
				return Entity{ entity, this };
		}

		return {};
	}

	void Scene::OnRuntimeStart()
	{
		m_IsRunning = true;

		// Create Physics Objects
		OnPhysics2DStart();

		// Create Scripts
		OnScriptsCreate();

		// Scripts OnStart
		OnScriptsStart();
	}

	void Scene::OnSimulationStart()
	{
		OnPhysics2DStart();
	}

	void Scene::OnRuntimeStop()
	{
		m_IsRunning = false;

		// Scripts On Destroy
		OnScriptsStop();

		// Destroy Physics Objects
		OnPhysics2DStop();
	}

	void Scene::OnSimulationStop()
	{
		OnPhysics2DStop();
	}

	void Scene::OnUpdateRuntime(Timestep ts)
	{
		if (!m_IsPaused || m_StepFrames-- > 0)
		{
			// Update scripts
			OnScriptsUpdate(ts);

			// Physics
			OnPhysics2DUpdate(ts);

			// Late Update scripts
			OnScriptsLateUpdate(ts);
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

			OnRender2DUpdate();

			Renderer2D::EndScene();
		}
	}

	void Scene::OnUpdateSimulation(Timestep ts, EditorCamera& camera)
	{
		if (!m_IsPaused || m_StepFrames-- > 0)
			OnPhysics2DUpdate(ts);

		Renderer2D::BeginScene(camera);

		OnRender2DUpdate();

		Renderer2D::EndScene();
	}

	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		Renderer2D::BeginScene(camera);

		OnRender2DUpdate();

		Renderer2D::EndScene();
	}

	void Scene::OnPhysics2DStart()
	{
		m_PhysicsWorld = new b2World({ 0.0f, -9.8f });
		m_PhysicsWorld->SetAutoClearForces(false);

		auto view = m_Registry.view<Rigidbody2DComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

			b2BodyDef bodyDef;
			bodyDef.type = Rigidbody2DTypeToBox2DBodyType(rb2d.Type);
			bodyDef.position.Set(transform.Position.x, transform.Position.y);
			bodyDef.angle = transform.Rotation.z;

			rb2d.previousPosition = transform.Position;
			rb2d.previousAngle = transform.Rotation.z;

			b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);

			body->SetFixedRotation(rb2d.FixedRotation);
			rb2d.RuntimeBody = body;

			if (entity.HasComponent<BoxCollider2DComponent>())
			{
				auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();

				b2PolygonShape boxShape;
				boxShape.SetAsBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y, b2Vec2(bc2d.Offset.x, bc2d.Offset.y), 0);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &boxShape;
				fixtureDef.density = bc2d.Density;
				fixtureDef.friction = bc2d.Friction;
				fixtureDef.restitution = bc2d.Restitution;
				fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;

				body->CreateFixture(&fixtureDef);
			}

			if (entity.HasComponent<CircleCollider2DComponent>())
			{
				auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();

				b2CircleShape circleShape;
				circleShape.m_p.Set(cc2d.Offset.x, cc2d.Offset.y);
				circleShape.m_radius = transform.Scale.x * cc2d.Radius;

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &circleShape;
				fixtureDef.density = cc2d.Density;
				fixtureDef.friction = cc2d.Friction;
				fixtureDef.restitution = cc2d.Restitution;
				fixtureDef.restitutionThreshold = cc2d.RestitutionThreshold;

				body->CreateFixture(&fixtureDef);
			}
		}
	}

	void Scene::OnScriptsCreate()
	{
		ScriptEngine::OnRuntimeStart(this);

		// Instantiate Script Entities
		auto view = m_Registry.view<ScriptComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			ScriptEngine::OnCreateEntity(entity);
		}
	}

	void Scene::OnScriptsStart()
	{
		// Scripts OnStart
		auto view = m_Registry.view<ScriptComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			ScriptEngine::OnStartEntity(entity);
		}
	}

	void Scene::OnPhysics2DStop()
	{
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;
	}

	void Scene::OnScriptsStop()
	{
		auto view = m_Registry.view<ScriptComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			ScriptEngine::OnDestroyEntity(entity);
		}

		ScriptEngine::OnRuntimeStop();
	}

	void Scene::OnScriptsUpdate(Timestep ts)
	{
		// Update Scripts
		auto view = m_Registry.view<ScriptComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			ScriptEngine::OnUpdateEntity(entity, ts);
		}

		// Update Native Scripts
		m_Registry.view<NativeScriptComponent>().each([=](auto e, auto& nsc)
		{
			if (!nsc.Instance)
			{
				nsc.Instance = nsc.InstantiateScript();
				nsc.Instance->m_Entity = Entity{ e, this };
				nsc.Instance->OnCreate();
			}

			nsc.Instance->OnUpdate(ts);
		});
	}

	// Fixed timestep guide: 
	// Original Article: https://gafferongames.com/post/fix_your_timestep/
	// Box2d Specific: https://www.unagames.com/blog/daniele/2010/06/fixed-time-step-implementation-box2d#:~:text=If%20you%20are%20interested%20in,with%20a%20variable%20frame%2Drate.
	void Scene::OnPhysics2DUpdate(Timestep ts)
	{
		const int maxSteps = 5;
		m_Accumulator += ts;
		const int nSteps = std::floor(m_Accumulator / m_PhysicsTimestep);
		if (nSteps > 0)
		{
			m_Accumulator -= nSteps * m_PhysicsTimestep;
		}
		m_AccumulatorRatio = m_Accumulator / m_PhysicsTimestep;

		auto view = m_Registry.view<Rigidbody2DComponent>();

		const int nStepsClamped = std::min(nSteps, maxSteps);
		for (int i = 0; i < nStepsClamped; ++i)
		{
			// reset smoothing
			for (auto e : view)
			{
				Entity entity = { e, this };
				auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

				b2Body* body = (b2Body*)rb2d.RuntimeBody;
				if (body->GetType() == b2_staticBody)
				{
					continue;
				}

				auto& transform = entity.GetComponent<TransformComponent>();
				switch (rb2d.Smoothing)
				{
					case Rigidbody2DComponent::SmoothingType::Interpolation:
					{
						auto& position = body->GetPosition();
						transform.Position = glm::vec3(position.x, position.y, transform.Position.z);
						rb2d.previousPosition = glm::vec2(position.x, position.y);
						transform.Rotation.z = rb2d.previousAngle = body->GetAngle();
						break;
					}
					case Rigidbody2DComponent::SmoothingType::Extrapolation:
					case Rigidbody2DComponent::SmoothingType::None:
					default:
					{
						auto& position = body->GetPosition();
						transform.Position = glm::vec3(position.x, position.y, transform.Position.z);
						transform.Rotation.z = body->GetAngle();
						break;
					}
				}
			}

			m_PhysicsWorld->Step(m_PhysicsTimestep, m_VelocityIteractions, m_PositionIteractions);
		}

		m_PhysicsWorld->ClearForces();

		// apply smoothing
		const float oneMinusRatio = 1.0f - m_AccumulatorRatio;

		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

			b2Body* body = (b2Body*)rb2d.RuntimeBody;
			if (body->GetType() == b2_staticBody)
			{
				continue;
			}

			auto& transform = entity.GetComponent<TransformComponent>();
			switch (rb2d.Smoothing)
			{
				case Rigidbody2DComponent::SmoothingType::Interpolation:
				{
					auto& position = m_AccumulatorRatio * body->GetPosition() + oneMinusRatio * b2Vec2(rb2d.previousPosition.x, rb2d.previousPosition.y);
					transform.Position = glm::vec3(position.x, position.y, transform.Position.z);
					transform.Rotation.z = m_AccumulatorRatio * body->GetAngle() + oneMinusRatio * rb2d.previousAngle;
					break;
				}
				case Rigidbody2DComponent::SmoothingType::Extrapolation:
				{
					auto& position = body->GetPosition() + ts * body->GetLinearVelocity();
					transform.Position = glm::vec3(position.x, position.y, transform.Position.z);
					transform.Rotation.z = body->GetAngle() + ts * body->GetAngularVelocity();
					break;
				}
				case Rigidbody2DComponent::SmoothingType::None:
				default:
				{
					auto& position = body->GetPosition();
					transform.Position = glm::vec3(position.x, position.y, transform.Position.z);
					transform.Rotation.z = body->GetAngle();
					break;
				}
			}
		}
	}

	void Scene::OnScriptsLateUpdate(Timestep ts)
	{
		// Late Update Scripts
		auto view = m_Registry.view<ScriptComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			ScriptEngine::OnLateUpdateEntity(entity, ts);
		}
	}

	void Scene::OnRender2DUpdate()
	{
		{ // Draw Sprites
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

				Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
			}
		}

		{ // Draw Circles
			auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
			for (auto entity : view)
			{
				auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);

				Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
			}
		}
	}

#pragma region OnComponentAdded
	template <typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		static_assert(sizeof(T) == 0);
	}

	template<>
	void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
	{
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
	void Scene::OnComponentAdded<CircleRendererComponent>(Entity entity, CircleRendererComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<CircleCollider2DComponent>(Entity entity, CircleCollider2DComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<ScriptComponent>(Entity entity, ScriptComponent& component)
	{
	}
#pragma endregion OnComponentAdded
}

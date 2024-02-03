#include "enginepch.h"
#include "Engine/Scene/Scene.h"

#include "Engine/Core/Application.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/ScriptableEntity.h"
#include "Engine/Scene/Prefab.h"
#include "Engine/Renderer/Renderer2D.h"
#include "Engine/Physics/Physics2D.h"
#include "Engine/Scripting/ScriptEngine.h"
#include "Engine/UI/UIEngine.h"

#include <glm/glm.hpp>

namespace Engine
{
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

		newScene->m_ScreenCamera = other->m_ScreenCamera;

		auto& srcSceneRegistry = other->m_Registry;
		auto& dstceneRegistry = newScene->m_Registry;

		// Create entities in new scene
		auto idView = srcSceneRegistry.view<IDComponent>();
		for (auto e : idView)
		{
			UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
			const auto& name = srcSceneRegistry.get<TagComponent>(e).Tag;
			auto& relationship = srcSceneRegistry.get<RelationshipComponent>(e);
			Entity newEntity = newScene->CreateEntityWithUUID(uuid, name);
			auto& newRelationship = newEntity.GetComponent<RelationshipComponent>();
			newRelationship = relationship;
		}

		// Copy Components (Except ID, Tag, and Relationship Components)
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
		entity.AddComponent<RelationshipComponent>();
		entity.AddComponent<TagComponent>().Tag = name.empty() ? "Entity" : name;

		m_EntityMap[uuid] = entity;

		if (m_IsRunning)
			ScriptEngine::InstantiateEntity(entity);

		return entity;
	}

	Entity Scene::CreateEntityFromPrefab(AssetHandle prefabHandle)
	{
		Ref<Prefab> prefab = AssetManager::GetAsset<Prefab>(prefabHandle);

		Entity entity = CopyEntityFromOtherScene(prefab->m_PrefabEntity);

		if (m_IsRunning)
			ScriptEngine::InstantiateEntity(entity);

		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		if (m_IsRunning)
		{
			Physics2DEngine::DestroyBody(entity);
			AudioEngine::StopSound(entity.GetUUID());
		}

		if (entity.GetComponent<RelationshipComponent>().Parent.IsValid())
		{
			Entity parent = entity.GetParent();
			parent.RemoveChild(entity);
		}

		for (auto& child : entity.Children())
		{
			entity.RemoveChild(child);
			DestroyEntity(child);
		}

		m_EntityMap.erase(entity.GetUUID());
		m_Registry.destroy(entity);
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		if (m_ViewportWidth == width && m_ViewportHeight == height)
			return;

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

		m_ScreenCamera.SetViewportSize(width, height);
		m_ScreenCamera.SetOrthographic(height, -1.0f, 1.0f);
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

	Entity Scene::DuplicateEntity(Entity entity)
	{
		std::string name = entity.GetName();
		Entity newEntity = CreateEntity(name);
		CopyComponentIfExists(AllComponents{}, newEntity, entity);

		auto& relationship = entity.GetComponent<RelationshipComponent>();

		if (relationship.HasChildren())
		{
			UUID childIterator = relationship.FirstChild;
			for (uint64_t i = 0; i < relationship.ChildrenCount; ++i)
			{
				Entity childEntity = GetEntityWithUUID(childIterator);
				Entity newChildEntity = DuplicateEntity(childEntity);
				newEntity.AddChild(newChildEntity);
				
				childIterator = childEntity.GetComponent<RelationshipComponent>().NextChild;
				if (!childIterator.IsValid())
					break;
			}
		}

		return newEntity;
	}

	bool Scene::DoesEntityExist(UUID uuid)
	{
		return m_EntityMap.find(uuid) != m_EntityMap.end();
	}

	bool Scene::IsEntityHandleValid(entt::entity handle)
	{
		return m_Registry.valid(handle);
	}

	Entity Scene::GetEntityWithUUID(UUID uuid)
	{
		ENGINE_CORE_ASSERT(DoesEntityExist(uuid), "Could not find Entity with UUID: " + std::to_string(uuid) + " in Scene: " + m_Name);
		return { m_EntityMap.at(uuid), this };
	}

	Entity Scene::FindEntityByName(const std::string_view& entityName)
	{
		auto view = m_Registry.view<TagComponent>();
		for (auto e : view)
		{
			const TagComponent& tc = view.get<TagComponent>(e);
			if (tc.Tag == entityName)
				return Entity{ e, this };
		}

		return {};
	}

	Entity Scene::CopyEntityFromOtherScene(Entity otherEntity)
	{
		std::string name = otherEntity.GetName();
		Entity newEntity = CreateEntity(name);
		CopyComponentIfExists(AllComponents{}, newEntity, otherEntity);

		auto& relationship = otherEntity.GetComponent<RelationshipComponent>();

		if (relationship.HasChildren())
		{
			UUID childIterator = relationship.FirstChild;
			Scene* otherScene = otherEntity.m_Scene;
			for (uint64_t i = 0; i < relationship.ChildrenCount; ++i)
			{
				Entity childEntity = otherScene->GetEntityWithUUID(childIterator);
				Entity newChildEntity = CopyEntityFromOtherScene(childEntity);
				newEntity.AddChild(newChildEntity);

				childIterator = childEntity.GetComponent<RelationshipComponent>().NextChild;
				if (!childIterator.IsValid())
					break;
			}
		}

		return newEntity;
	}

	void Scene::OnRuntimeStart()
	{
		m_IsRunning = true;

		// Start UI
		OnUIStart();

		// Create Physics Objects
		OnPhysics2DStart();

		// Create Scripts
		OnScriptsCreate();

		// Scripts OnStart
		OnScriptsStart();

		// Start Audio on Start
		m_Registry.view<AudioSourceComponent>().each([=](auto e, const auto& source)
		{
			Entity entity = { e, this };
			if (source.AutoPlayOnStart)
			{
				AudioEngine::PlaySound(entity.GetUUID(), source.Clip, source.Params);
			}
		});
	}

	void Scene::OnSimulationStart()
	{
		OnPhysics2DStart();
	}

	void Scene::OnRuntimeStop()
	{
		m_IsRunning = false;

		// UI Stop
		OnUIStop();

		// Scripts On Destroy
		OnScriptsStop();

		// Destroy Physics Objects
		OnPhysics2DStop();

		// Stop Audio
		m_Registry.view<AudioSourceComponent>().each([=](auto e, const auto& source)
		{
			Entity entity = { e, this };
			AudioEngine::StopSound(entity.GetUUID());
		});
	}

	void Scene::OnSimulationStop()
	{
		OnPhysics2DStop();
	}

	void Scene::OnUpdateRuntime(Timestep ts)
	{
		if (!m_IsPaused || m_StepFrames-- > 0)
		{
			// Update UI
			OnUIUpdate(ts);

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
			auto view = m_Registry.view<CameraComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };
				auto& camera = entity.GetComponent<CameraComponent>();
				
				if (camera.Primary)
				{
					mainCamera = &camera.Camera;
					cameraTransform = entity.GetWorldSpaceTransform();
					break;
				}
			}
		}

		// world space render
		if (mainCamera)
		{
			Renderer2D::BeginScene(*mainCamera, cameraTransform);

			OnRender2DUpdate();

			Renderer2D::EndScene();
		}

		Renderer2D::BeginScene(m_ScreenCamera, glm::mat4(1.0f));

		OnRenderUIUpdate();

		Renderer2D::EndScene();

		// Pause Audio Playback
		AudioEngine::PausePlayback(m_IsPaused);
	}

	void Scene::OnUpdateSimulation(Timestep ts, EditorCamera& camera)
	{
		if (!m_IsPaused || m_StepFrames-- > 0)
			OnPhysics2DUpdate(ts);

		Renderer2D::BeginScene(camera);

		OnRender2DUpdate();

		Renderer2D::EndScene();

		Renderer2D::BeginScene(m_ScreenCamera, glm::mat4(1.0f));

		OnRenderUIUpdate();

		Renderer2D::EndScene();
	}

	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		Renderer2D::BeginScene(camera);

		OnRender2DUpdate();

		Renderer2D::EndScene();

		Renderer2D::BeginScene(m_ScreenCamera, glm::mat4(1.0f));

		OnRenderUIUpdate();

		Renderer2D::EndScene();
	}

	void Scene::OnUIStart()
	{
		UIEngine::OnUIStart();
	}

	void Scene::OnPhysics2DStart()
	{
		Physics2DEngine::OnPhysicsStart();
	}

	void Scene::OnScriptsCreate()
	{
		for (const auto& [handle, metadata] : AssetManager::GetAssets()) // or asset pak
		{
			if (metadata.Type == AssetType::Prefab) // TODO remove once all asset types can work
			{
				ScriptEngine::InstantiateAsset(handle);
			}
		}

		// Instantiate Entities in Script Engine
		auto view = m_Registry.view<TransformComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			ScriptEngine::InstantiateEntity(entity);
		}

		// Script OnCreate
		m_Registry.view<ScriptComponent>().each([=](auto e, const auto& sc)
		{
			Entity entity = { e, this };
			ScriptEngine::OnCreateEntity(entity, sc);
		});
	}

	void Scene::OnScriptsStart()
	{
		// Scripts OnStart
		m_Registry.view<ScriptComponent>().each([=](auto e, const auto& sc)
		{
			Entity entity = { e, this };
			ScriptEngine::OnStartEntity(entity, sc);
		});
	}

	void Scene::OnUIStop()
	{
		UIEngine::OnUIStop();
	}

	void Scene::OnPhysics2DStop()
	{
		Physics2DEngine::OnPhysicsStop();
	}

	void Scene::OnScriptsStop()
	{
		m_Registry.view<ScriptComponent>().each([=](auto e, const auto& sc)
		{
			Entity entity = { e, this };
			ScriptEngine::OnDestroyEntity(entity, sc);
		});
	}

	void Scene::OnUIUpdate(Timestep ts)
	{
		UIEngine::OnUIUpdate(ts, m_ViewportWidth, m_ViewportHeight);
	}

	void Scene::OnScriptsUpdate(Timestep ts)
	{
		// Update Scripts
		m_Registry.view<ScriptComponent>().each([=](auto e, const auto& sc)
		{
			Entity entity = { e, this };
			ScriptEngine::OnUpdateEntity(entity, sc, ts);
		});

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

	void Scene::OnPhysics2DUpdate(Timestep ts)
	{
		Physics2DEngine::OnPhysicsUpdate(ts);
	}

	void Scene::OnScriptsLateUpdate(Timestep ts)
	{
		// Late Update Scripts
		m_Registry.view<ScriptComponent>().each([=](auto e, const auto& sc)
		{
			Entity entity = { e, this };
			ScriptEngine::OnLateUpdateEntity(entity, sc, ts);
		});
	}

	void Scene::OnRender2DUpdate()
	{
		// Draw Sprites
		m_Registry.view<SpriteRendererComponent>(entt::exclude<UILayoutComponent>).each([=](auto e, auto& sprite)
		{
			Entity entity = { e, this };
			Renderer2D::DrawSprite(entity.GetWorldSpaceTransform(), sprite, (int)e);
		});

		// Draw Circles
		m_Registry.view<CircleRendererComponent>(entt::exclude<UILayoutComponent>).each([=](auto e, auto& circle)
		{
			Entity entity = { e, this };
			Renderer2D::DrawCircle(entity.GetWorldSpaceTransform(), circle.Color, circle.Thickness, circle.Fade, (int)e);
		});

		// Draw Text
		m_Registry.view<TextRendererComponent>(entt::exclude<UILayoutComponent>).each([=](auto e, auto& trc)
		{
			Entity entity = { e, this };
			Renderer2D::DrawString(trc.TextString, entity.GetWorldSpaceTransform(), trc, (int)e);
		});
	}

	void Scene::OnRenderUIUpdate()
	{
		{ // Draw Images
			auto view = m_Registry.view<UILayoutComponent, SpriteRendererComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };
				SpriteRendererComponent sprite = entity.GetComponent<SpriteRendererComponent>();

				if (entity.HasComponent<UIButtonComponent>())
				{
					UIButtonComponent uiButton = entity.GetComponent<UIButtonComponent>();
					sprite.Color *= uiButton.GetButtonTint();
				}

				Renderer2D::DrawSprite(entity.GetUISpaceTransform(), sprite, (int)e);
			}
		}

		{ // Draw Circles
			auto view = m_Registry.view<UILayoutComponent, CircleRendererComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };
				CircleRendererComponent circle = entity.GetComponent<CircleRendererComponent>();
				Renderer2D::DrawCircle(entity.GetUISpaceTransform(), circle.Color, circle.Thickness, circle.Fade, (int)e);
			}
		}

		{ // Draw Text
			auto view = m_Registry.view<UILayoutComponent, TextRendererComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };
				TextRendererComponent trc = entity.GetComponent<TextRendererComponent>();
				Renderer2D::DrawString(trc.TextString, entity.GetUISpaceTransform(), trc, (int)e);
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
	void Scene::OnComponentAdded<PrefabComponent>(Entity entity, PrefabComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<RelationshipComponent>(Entity entity, RelationshipComponent& component)
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
		if (m_IsRunning)
			component.RuntimeBody = Physics2DEngine::CreateRigidbody(entity);
	}

	template<>
	void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
	{
		if (m_IsRunning)
		{
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
			component.RuntimeFixture = Physics2DEngine::CreateCollider(transform, rb2d, component);
		}
	}

	template<>
	void Scene::OnComponentAdded<CircleCollider2DComponent>(Entity entity, CircleCollider2DComponent& component)
	{
		if (m_IsRunning)
		{
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
			component.RuntimeFixture = Physics2DEngine::CreateCollider(transform, rb2d, component);
		}
	}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<ScriptComponent>(Entity entity, ScriptComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<TextRendererComponent>(Entity entity, TextRendererComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<AudioSourceComponent>(Entity entity, AudioSourceComponent& component)
	{
		if (m_IsRunning)
		{
			auto& source = entity.GetComponent<AudioSourceComponent>();
			if (source.AutoPlayOnStart)
			{
				AudioEngine::PlaySound(entity.GetUUID(), component.Clip, component.Params);
			}
		}
	}

	template<>
	void Scene::OnComponentAdded<UILayoutComponent>(Entity entity, UILayoutComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<UIButtonComponent>(Entity entity, UIButtonComponent& component)
	{
	}
#pragma endregion OnComponentAdded
}

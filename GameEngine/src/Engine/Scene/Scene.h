#pragma once

#include "Engine/Core/Timestep.h"
#include "Engine/Renderer/EditorCamera.h"
#include "Engine/Core/UUID.h"

#include <entt.hpp>

class b2World;

namespace Engine
{
	class Entity;
	
	class Scene
	{
	public:
		Scene();
		Scene(std::string name);
		~Scene();
		
		static Ref<Scene> Copy(Ref<Scene> other);

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
		void DestroyEntity(Entity entity);
		
		void OnRuntimeStart();
		void OnSimulationStart();

		void OnRuntimeStop();
		void OnSimulationStop();

		void OnUpdateRuntime(Timestep ts);
		void OnUpdateSimulation(Timestep ts, EditorCamera& camera);
		void OnUpdateEditor(Timestep ts, EditorCamera& camera);

		void OnViewportResize(uint32_t width, uint32_t height);
		Entity GetPrimaryCameraEntity();

		void DuplicateEntity(Entity entity);
		Entity GetEntityWithUUID(UUID uuid);

		void SetSceneName(const std::string& name) { if (!name.empty()) m_Name = name; }

		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}

	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

		void OnPhysics2DStart();
		void OnScriptsStart();

		void OnPhysics2DStop();
		void OnScriptsStop();

		void OnPhysics2DUpdate(Timestep ts);
		void OnScriptsUpdate(Timestep ts);
		void OnRender2DUpdate();

	private:
		entt::registry m_Registry;
		std::unordered_map<UUID, entt::entity> m_EntityMap;

		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
		std::string m_Name;

		// Physics
		float m_Accumulator = 0.0f;
		const float m_PhysicsTimestep = 0.01f;
		b2World* m_PhysicsWorld;
		uint32_t m_VelocityIteractions = 8;
		uint32_t m_PositionIteractions = 3;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};
}

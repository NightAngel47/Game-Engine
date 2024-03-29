#pragma once

#include "Engine/Core/Timestep.h"
#include "Engine/Core/UUID.h"
#include "Engine/Renderer/EditorCamera.h"
#include "Engine/Scene/SceneCamera.h"

#include <entt.hpp>

class b2World;

namespace Engine
{
	class Entity;
	
	class Scene
	{
	public:
		Scene() = default;
		Scene(std::string name)
			:m_Name(name) {};
		~Scene() = default;
		
		static Ref<Scene> Copy(Ref<Scene> other);
		void SetSceneName(const std::string& name) { if (!name.empty()) m_Name = name; }

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
		void DestroyEntity(Entity entity);

		void OnViewportResize(uint32_t width, uint32_t height);
		Entity GetPrimaryCameraEntity();
		SceneCamera GetScreenCamera() const { return m_ScreenCamera; }

		void Step(int frames = 1);

		Entity DuplicateEntity(Entity entity);
		bool DoesEntityExist(UUID uuid);
		Entity GetEntityWithUUID(UUID uuid);
		Entity FindEntityByName(const std::string_view& entityName);
		
		// Start Play/Sim Whole
		void OnRuntimeStart();
		void OnSimulationStart();

		// Stop Play/Sim Whole
		void OnRuntimeStop();
		void OnSimulationStop();

		// Update Play/Sim Whole
		void OnUpdateRuntime(Timestep ts);
		void OnUpdateSimulation(Timestep ts, EditorCamera& camera);
		void OnUpdateEditor(Timestep ts, EditorCamera& camera);

		bool IsRunning() const { return m_IsRunning; }
		bool IsPaused() const { return m_IsPaused; }

		void SetPaused(bool paused) { m_IsPaused = paused; }

		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}

	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

		// Start Play/Sim Section
		void OnUIStart();
		void OnPhysics2DStart();
		void OnScriptsCreate();
		void OnScriptsStart();

		// Stop Play/Sim Section
		void OnUIStop();
		void OnPhysics2DStop();
		void OnScriptsStop();

		// Update Play/Sim Section
		void OnUIUpdate(Timestep ts);
		void OnScriptsUpdate(Timestep ts);
		void OnPhysics2DUpdate(Timestep ts);
		void OnScriptsLateUpdate(Timestep ts);
		void OnRender2DUpdate();
		void OnRenderUIUpdate();
	private:
		entt::registry m_Registry;
		std::unordered_map<UUID, entt::entity> m_EntityMap;

		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
		SceneCamera m_ScreenCamera;

		std::string m_Name;
		
		bool m_IsRunning = false;
		bool m_IsPaused = false;
		int m_StepFrames = 0;

		friend class Entity;
		friend class SceneSerializer;
		friend class EntitySerializer;
		friend class PrefabSerializer;
		friend class SceneHierarchyPanel;
	};
}

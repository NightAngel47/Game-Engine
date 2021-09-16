#pragma once

#include "Engine/Core/Timestep.h"
#include "Engine/Renderer/EditorCamera.h"

#include <entt.hpp>
#include <box2d/box2d.h>

namespace Engine
{
	class Entity;
	
	class Scene
	{
	public:
		Scene();
		Scene(std::string name);
		~Scene();
		
		Entity CreateEntity(const std::string& name = std::string());
		void DestroyEntity(Entity entity);
		
		void OnScenePlay();
		void OnSceneStop();

		void OnUpdateRuntime(Timestep ts);
		void OnUpdatePhysics(Timestep ts = 1.0f / 60.0f);
		void OnUpdateEditor(Timestep ts, EditorCamera& camera);

		void OnViewportResize(uint32_t width, uint32_t height);
		Entity GetPrimaryCameraEntity();

		void SetSceneName(const std::string& name) { if (!name.empty()) m_Name = name; }
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
		std::string m_Name;

		// Physics
		b2World* m_World;
		b2Vec2 m_Gravity{ 0.0f, -10.0f };
		uint32_t m_VelocityIteractions = 8;
		uint32_t m_PositionIteractions = 3;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};
}

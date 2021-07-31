#pragma once

#include "Engine/Core/Timestep.h"
#include "Engine/Renderer/EditorCamera.h"

#include <entt.hpp>

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
		
		void OnUpdateRuntime(Timestep ts);
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

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};
}

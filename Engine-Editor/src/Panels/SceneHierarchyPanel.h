#pragma once
#include <Engine.h>

namespace Engine
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel()
			: m_SelectionContext(UUID::INVALID()) {};
		SceneHierarchyPanel(const Ref<Scene>& context);

		void SetContext(const Ref<Scene>& context);

		void OnImGuiRender();

		bool IsSelectedEntityValid() { return m_SelectionContext.IsValid() && m_Context->DoesEntityExist(m_SelectionContext); }
		Entity GetSelectedEntity()
		{ 
			if (!IsSelectedEntityValid())
				return {};

			return m_Context->GetEntityWithUUID(m_SelectionContext); 
		}
		void SetSelectedEntity(Entity entity);
		void SetSelectedEntity(UUID entityID);
	private:
		template<typename T>
		void DisplayAddComponentEntry(const std::string& entryName);

		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);

		void SavePrefabAs();
		void CreateFromPrefab();
		void CreateFromPrefab(const std::filesystem::path& filepath);
		void CreateChildEntity();
	private:
		Ref<Scene> m_Context;
		UUID m_SelectionContext = UUID::INVALID();
	};
}

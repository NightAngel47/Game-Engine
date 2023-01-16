#pragma once
#include <Engine.h>

namespace Engine
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& context);

		void SetContext(const Ref<Scene>& context);

		void OnImGuiRender();
		Entity GetSelectedEntity() const 
		{
			if (!m_Context->DoesEntityExist(m_SelectionContext))
				return {};

			return m_Context->GetEntityWithUUID(m_SelectionContext); 
		}
		void SetSelectedEntity(Entity entity);
	private:
		template<typename T>
		void DisplayAddComponentEntry(const std::string& entryName);

		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
	private:
		Ref<Scene> m_Context;
		UUID m_SelectionContext;
	};
}

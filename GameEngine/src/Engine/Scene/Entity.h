#pragma once

#include "Engine/Scene/Scene.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/UUID.h"
#include "Engine/Scene/Components.h"

#include <entt.hpp>
#include <glm/glm.hpp>

namespace Engine
{
	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene)
			:m_EntityHandle(handle), m_Scene(scene) {}
		Entity(const Entity& other) = default;

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			ENGINE_CORE_ASSERT(!HasComponent<T>(), "Entity already has componenet!");
			T& component = m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<T>(*this, component);
			return component;
		}

		template<typename T, typename... Args>
		T& AddOrReplaceComponent(Args&&... args)
		{
			T& component = m_Scene->m_Registry.emplace_or_replace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<T>(*this, component);
			return component;
		}

		template<typename T>
		T& GetComponent()
		{
			ENGINE_CORE_ASSERT(HasComponent<T>(), "Entity does not have componenet!");
			
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}
		
		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.has<T>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponent()
		{
			ENGINE_CORE_ASSERT(HasComponent<T>(), "Entity does not have componenet!");
			
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		operator bool() const { return m_EntityHandle != entt::null; }
		operator entt::entity() const { return m_EntityHandle; }
		operator uint32_t() const { return (uint32_t)m_EntityHandle; }

		UUID GetUUID() { return GetComponent<IDComponent>().ID; }
		const std::string& GetName() { return GetComponent<TagComponent>().Tag; }

		glm::mat4 GetWorldTransform()
		{
			TransformComponent transform = GetComponent<TransformComponent>();
			glm::mat4 localTransform = transform.GetTransform();

			UUID parentID = GetComponent<RelationshipComponent>().Parent;
			while (parentID.IsValid())
			{
				Entity parentEntity = m_Scene->GetEntityWithUUID(parentID);
				localTransform = parentEntity.GetComponent<TransformComponent>().GetTransform() * localTransform;
				parentID = parentEntity.GetComponent<RelationshipComponent>().Parent;
			}

			return localTransform;
		}

		Entity GetParent() { return m_Scene->GetEntityWithUUID(GetComponent<RelationshipComponent>().Parent); }

		void AddChild(Entity child)
		{
			auto& childRelationship = child.GetComponent<RelationshipComponent>();

			// Remove child entity references
			if (childRelationship.Parent.IsValid())
				child.GetParent().RemoveChild(child);
			childRelationship.Parent = GetUUID();

			auto& relationship = GetComponent<RelationshipComponent>();
			// Update parent and existing children
			if (relationship.HasChildren())
			{
				UUID childIterator = relationship.FirstChild;
				for (uint64_t i{}; i < relationship.ChildrenCount; ++i)
				{
					if (!childIterator.IsValid())
						break;

					Entity childIteratorEntity = m_Scene->GetEntityWithUUID(childIterator);
					auto& childIteratorRelationship = childIteratorEntity.GetComponent<RelationshipComponent>();

					// Found the end, add new child
					if (!childIteratorRelationship.NextChild.IsValid())
					{
						childIteratorRelationship.NextChild = child.GetUUID();
						childRelationship.PrevChild = childIterator;
						relationship.ChildrenCount++;
						return;
					}

					// Check next child if not found
					childIterator = childIteratorRelationship.NextChild;
				}
			}

			// Parent had no existing children, so adding this as first
			relationship.FirstChild = child.GetUUID();
			relationship.ChildrenCount++;
		}

		bool RemoveChild(Entity child)
		{
			// Remove passed child entity from this entity (parent)
			auto& relationship = GetComponent<RelationshipComponent>();
			if (!relationship.HasChildren())
				return false;

			UUID childID = child.GetUUID();
			UUID childIterator = relationship.FirstChild;
			for (uint64_t i{}; i < relationship.ChildrenCount; ++i)
			{
				if (!childIterator.IsValid())
					break;

				// Check if found child
				if (childIterator == childID)
				{
					auto& childRelationship = child.GetComponent<RelationshipComponent>();
					UUID nextChild = childRelationship.NextChild;
					UUID prevChild = childRelationship.PrevChild;

					// Get Next Child Relationship
					auto& nextChildRelationship = RelationshipComponent{};
					if (nextChild.IsValid())
					{
						Entity nextChildEntity = m_Scene->GetEntityWithUUID(nextChild);
						nextChildRelationship = nextChildEntity.GetComponent<RelationshipComponent>();
					}

					// Get Prev Child Relationship
					auto& prevChildRelationship = RelationshipComponent{};
					if (prevChild.IsValid())
					{
						Entity prevChildEntity = m_Scene->GetEntityWithUUID(prevChild);
						prevChildRelationship = prevChildEntity.GetComponent<RelationshipComponent>();
					}

					// Update references
					if (prevChild.IsValid())
					{
						if (nextChild.IsValid())	// Middle Child Removed
						{
							prevChildRelationship.NextChild = nextChild;
							nextChildRelationship.PrevChild = prevChild;
						}
						else						// Last Child Removed
						{
							prevChildRelationship.NextChild = UUID::INVALID();
						}
					}
					else
					{
						if (nextChild.IsValid())	// First Child Removed with Valid Next
						{
							relationship.FirstChild = nextChild;
							nextChildRelationship.PrevChild = UUID::INVALID();
						}
						else						// First and Only Child Removed
						{
							relationship.FirstChild = UUID::INVALID();
						}
					}

					relationship.ChildrenCount--;

					childRelationship.Parent = UUID::INVALID();
					childRelationship.NextChild = UUID::INVALID();
					childRelationship.PrevChild = UUID::INVALID();

					return true;
				}

				// Check next child if not found
				Entity childIteratorEntity = m_Scene->GetEntityWithUUID(childIterator);
				childIterator = childIteratorEntity.GetComponent<RelationshipComponent>().NextChild;
			}

			return false;
		}

		const std::vector<Entity> Children()
		{
			if (!HasComponent<RelationshipComponent>())
				return {};

			auto& relationship = GetComponent<RelationshipComponent>();
			std::vector<Entity> childEntities = std::vector<Entity>();

			if (!relationship.HasChildren())
				return childEntities;

			UUID childIterator = relationship.FirstChild;
			for (std::size_t i{}; i < relationship.ChildrenCount; ++i)
			{
				if (!childIterator.IsValid())
					break;

				Entity childIteratorEntity = m_Scene->GetEntityWithUUID(childIterator);
				childEntities.emplace_back(childIteratorEntity);

				// Proceed to next child
				childIterator = childIteratorEntity.GetComponent<RelationshipComponent>().NextChild;
			}

			return childEntities;
		}

		bool operator==(const Entity other) const { return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene; }
		bool operator!=(const Entity other) const { return !(*this == other); }
	private:
		entt::entity m_EntityHandle{ entt::null };
		Scene* m_Scene = nullptr;
	};
}

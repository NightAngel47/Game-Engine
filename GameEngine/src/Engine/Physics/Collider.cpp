#include "enginepch.h"
#include "Engine/Physics/Collider.h"

namespace Engine
{
	BoxCollider::BoxCollider(float width, float height)
	{
		m_Extents = glm::vec2(width, height);
		SetExtents(m_Extents);
	}

	BoxCollider::BoxCollider(const glm::vec2& extents)
	{
		m_Extents = extents;
		SetExtents(m_Extents);
	}

	void BoxCollider::SetExtents(float width, float height)
	{
		m_Shape.SetAsBox(width, height);
	}

	void BoxCollider::SetExtents(const glm::vec2& extents)
	{
		SetExtents(extents.x, extents.y);
	}

}

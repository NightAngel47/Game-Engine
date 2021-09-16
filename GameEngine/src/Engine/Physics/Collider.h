#pragma once

#include <box2d/box2d.h>
#include <glm/glm.hpp>

namespace Engine
{
	class Collider
	{
	public:
		virtual ~Collider() = default;

		virtual b2PolygonShape& GetShape() { return m_Shape; }
		virtual b2FixtureDef& GetFixtureDef() { return m_FixtureDef; }
	protected:
		b2PolygonShape m_Shape;
		b2FixtureDef m_FixtureDef;
	private:
		friend class Rigidbody;
	};

	class BoxCollider : Collider
	{
	public:
		BoxCollider() = default;
		BoxCollider(float width, float height);
		BoxCollider(const glm::vec2& extents);
		~BoxCollider() = default;

		b2PolygonShape& GetShape() override { return __super::GetShape(); }
		b2FixtureDef& GetFixtureDef() override { return __super::GetFixtureDef(); };

		void SetExtents(float width, float height);
		void SetExtents(const glm::vec2& extents);
		const glm::vec2& const GetExtents() { return m_Extents; };
	private:
		glm::vec2 m_Extents;
	};
}

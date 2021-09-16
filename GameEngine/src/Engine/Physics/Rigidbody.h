#pragma once

#include <box2d/box2d.h>
#include <glm/glm.hpp>

namespace Engine
{
	enum class PhysicsBodyType {
		Static = 0, Kinematic, Dynamic
	};

	// TODO change Rigidbody to be created on start vs when added to entity
	class Rigidbody
	{
	public:
		Rigidbody() = default;
		~Rigidbody() = default;

		void SetBodyType(Engine::PhysicsBodyType bodyType);

		Engine::PhysicsBodyType const GetBodyType();

		void SetPosition(const glm::vec2& position);
		const glm::vec2& const GetPosition();

		void SetAngle(const float angle);
		float const GetAngle();
		
		void CreateBody(b2World& world);
		void DestroyBody(b2World& world);

		void CreateFixture(const b2FixtureDef& fixtureDef);
		void CreateFixture(const b2Shape& shape, float density = 1.0f, float friction = 0.3f);
	private:
		const b2BodyType& const Getb2BodyType();
	private:
		b2Body* m_Body;
		b2BodyDef m_BodyDef;
	};
}

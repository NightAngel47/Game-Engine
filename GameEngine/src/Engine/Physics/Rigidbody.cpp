#include "enginepch.h"
#include "Engine/Physics/Rigidbody.h"

namespace Engine
{

	void Rigidbody::SetBodyType(Engine::PhysicsBodyType bodyType)
	{
		b2BodyType newType = Getb2BodyType();

		switch (bodyType)
		{
		case Engine::PhysicsBodyType::Static:
			newType = b2_staticBody;
			break;
		case Engine::PhysicsBodyType::Kinematic:
			newType = b2_kinematicBody;
			break;
		case Engine::PhysicsBodyType::Dynamic:
			newType = b2_dynamicBody;
			break;
		}

		if (m_Body) 
			m_Body->SetType(newType);
		else
			m_BodyDef.type = newType;

	}

	Engine::PhysicsBodyType const Rigidbody::GetBodyType()
	{
		switch (Getb2BodyType())
		{
		case b2_staticBody:
			return Engine::PhysicsBodyType::Static;
		case b2_kinematicBody:
			return Engine::PhysicsBodyType::Kinematic;
		case b2_dynamicBody:
			return Engine::PhysicsBodyType::Dynamic;
		}
	}

	void Rigidbody::SetPosition(const glm::vec2& position)
	{
		if (m_Body)
		{
			m_Body->SetTransform(b2Vec2(position.x, position.y), m_Body->GetAngle());
		}
		else
		{
			m_BodyDef.position.Set(position.x, position.y);
		}
	}


	const glm::vec2& const Rigidbody::GetPosition()
	{
		b2Vec2 position = b2Vec2();

		if (m_Body)
		{
			position = m_Body->GetWorldPoint(b2Vec2(0.0f, 0.0f));
		}
		else
		{
			position = m_BodyDef.position;
		}

		return glm::vec2(position.x, position.y);
	}


	void Rigidbody::SetAngle(const float angle)
	{
		if (m_Body)
		{
			m_Body->SetTransform(m_Body->GetTransform().p, angle);
		}
		else
		{
			m_BodyDef.angle = angle;
		}
	}


	float const Rigidbody::GetAngle()
	{
		float angle = 0.0f;

		if (m_Body)
		{
			angle = m_Body->GetAngle();
		}
		else
		{
			angle = m_BodyDef.angle;
		}
		
		return angle;
	}


	void Rigidbody::CreateBody(b2World& world)
	{
		m_Body = world.CreateBody(&m_BodyDef);
	}


	void Rigidbody::DestroyBody(b2World& world)
	{
		world.DestroyBody(m_Body);
		m_Body = nullptr;
	}

	void Rigidbody::CreateFixture(const b2FixtureDef& fixtureDef)
	{
		ENGINE_ASSERT(m_Body, "Can't create fixture before the world creates the body!");
		m_Body->CreateFixture(&fixtureDef);
	}


	void Rigidbody::CreateFixture(const b2Shape& shape, float density /*= 1.0f*/, float friction /*= 0.3f*/)
	{
		b2FixtureDef fixtureDef;
		fixtureDef.shape = &shape;
		fixtureDef.density = density;
		fixtureDef.friction = friction;
		CreateFixture(fixtureDef);
	}


	const b2BodyType& const Rigidbody::Getb2BodyType()
	{
		return m_Body ? m_Body->GetType() : m_BodyDef.type;
	}

}

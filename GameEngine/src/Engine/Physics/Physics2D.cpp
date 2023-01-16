#include "enginepch.h"
#include "Engine/Physics/Physics2D.h"

#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include "box2d/b2_circle_shape.h"

namespace Engine
{
	static b2FixtureDef CreateFixtureDef(const b2Shape& shape, const float density, const float friction, const float restitution, const float restitutionThreshold)
	{
		b2FixtureDef fixtureDef;
		fixtureDef.shape = &shape;
		fixtureDef.density = density;
		fixtureDef.friction = friction;
		fixtureDef.restitution = restitution;
		fixtureDef.restitutionThreshold = restitutionThreshold;
		return fixtureDef;
	}

	b2Body* Physics2D::CreateRigidbody(const TransformComponent& transform, Rigidbody2DComponent& rb2d, b2World* world)
	{
		b2BodyDef bodyDef;
		bodyDef.type = Utils::Rigidbody2DTypeToBox2DBodyType(rb2d.Type);
		bodyDef.position.Set(transform.Position.x, transform.Position.y);
		bodyDef.angle = transform.Rotation.z;

		rb2d.previousPosition = transform.Position;
		rb2d.previousAngle = transform.Rotation.z;

		b2Body* body = world->CreateBody(&bodyDef);

		body->SetFixedRotation(rb2d.FixedRotation);

		return body;
	}

	b2Fixture* Physics2D::CreateCollider(const TransformComponent& transform, const Rigidbody2DComponent& rb2d, const BoxCollider2DComponent& bc2d)
	{
		b2PolygonShape boxShape;
		boxShape.SetAsBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y, b2Vec2(bc2d.Offset.x, bc2d.Offset.y), 0);
		b2FixtureDef fixtureDef = CreateFixtureDef(boxShape, bc2d.Density, bc2d.Friction, bc2d.Restitution, bc2d.RestitutionThreshold);

		b2Body* body = (b2Body*)rb2d.RuntimeBody;
		return body->CreateFixture(&fixtureDef);
	}


	b2Fixture* Physics2D::CreateCollider(const TransformComponent& transform, const Rigidbody2DComponent& rb2d, const CircleCollider2DComponent& cc2d)
	{
		b2CircleShape circleShape;
		circleShape.m_p.Set(cc2d.Offset.x, cc2d.Offset.y);
		circleShape.m_radius = transform.Scale.x * cc2d.Radius;
		b2FixtureDef fixtureDef = CreateFixtureDef(circleShape, cc2d.Density, cc2d.Friction, cc2d.Restitution, cc2d.RestitutionThreshold);

		b2Body* body = (b2Body*)rb2d.RuntimeBody;
		return body->CreateFixture(&fixtureDef);
	}

}

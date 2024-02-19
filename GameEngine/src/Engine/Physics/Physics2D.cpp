#include "enginepch.h"
#include "Engine/Physics/Physics2D.h"
#include "Engine/Scripting/ScriptEngine.h"
#include "Engine/Scene/SceneManager.h"

#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include "box2d/b2_circle_shape.h"

namespace Engine
{
	Physics2DContactListener g_ContactListener;

	struct PhysicsWorldSettings
	{
		const float PHYSICS_TIMESTEP = 1.0f / 60.0f;
		const uint32_t VELOCITY_INTERACTIONS = 20;
		const uint32_t POSITION_INTERACTIONS = 16;
	};

	struct Physics2DEngineData
	{
		b2World* PhysicsWorld = nullptr;
		float Accumulator = 0.0f;
		float AccumulatorRatio = 0.0f;
		PhysicsWorldSettings Settings;

		std::vector<b2Body*> QueuedBodiesToDestroy = std::vector<b2Body*>();
		std::unordered_map<b2Body*, glm::vec2> QueuedBodiesToPosition = std::unordered_map<b2Body*, glm::vec2>();
		std::unordered_map<b2Body*, float> QueuedBodiesToRotate = std::unordered_map<b2Body*, float>();
	};

	static Physics2DEngineData* s_physics2DEngineData = nullptr;

	static b2FixtureDef CreateFixtureDef(const b2Shape& shape, const float density, const float friction, const float restitution, const float restitutionThreshold, const bool sensor)
	{
		b2FixtureDef fixtureDef;
		fixtureDef.shape = &shape;
		fixtureDef.density = density;
		fixtureDef.friction = friction;
		fixtureDef.restitution = restitution;
		fixtureDef.restitutionThreshold = restitutionThreshold;
		fixtureDef.isSensor = sensor;
		return fixtureDef;
	}

	static void DestroyQueuedBodiesToDestroy()
	{
		if (s_physics2DEngineData->QueuedBodiesToDestroy.empty())
			return;

		for (b2Body* body : s_physics2DEngineData->QueuedBodiesToDestroy)
		{
			if (body)
				s_physics2DEngineData->PhysicsWorld->DestroyBody(body);
		}

		s_physics2DEngineData->QueuedBodiesToDestroy.clear();
	}

	static void SetPositionQueuedBodiesToPosition()
	{
		if (s_physics2DEngineData->QueuedBodiesToPosition.empty())
			return;

		for (const auto& [body, position] : s_physics2DEngineData->QueuedBodiesToPosition)
		{
			if (body)
				body->SetTransform({ position.x, position.y }, body->GetAngle());
		}

		s_physics2DEngineData->QueuedBodiesToPosition.clear();
	}

	static void SetRotationQueuedBodiesToRotate()
	{
		if (s_physics2DEngineData->QueuedBodiesToRotate.empty())
			return;

		for (const auto& [body, angle] : s_physics2DEngineData->QueuedBodiesToRotate)
		{
			if (body)
				body->SetTransform(body->GetPosition(), angle);
		}

		s_physics2DEngineData->QueuedBodiesToRotate.clear();
	}

	void Physics2DEngine::OnPhysicsStart()
	{
		s_physics2DEngineData = new Physics2DEngineData();

		s_physics2DEngineData->PhysicsWorld = new b2World({ 0.0f, -9.8f });
		s_physics2DEngineData->PhysicsWorld->SetAllowSleeping(true);
		s_physics2DEngineData->PhysicsWorld->SetAutoClearForces(false);

		auto view = SceneManager::GetActiveScene()->GetAllEntitiesWith<Rigidbody2DComponent>();
		for (auto e : view)
		{
			Entity entity = { e, SceneManager::GetActiveScene().get() };
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

			rb2d.RuntimeBody = CreateRigidbody(entity);

			if (entity.HasComponent<BoxCollider2DComponent>())
			{
				auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
				CreateCollider(transform, rb2d, bc2d);
			}

			if (entity.HasComponent<CircleCollider2DComponent>())
			{
				auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();
				CreateCollider(transform, rb2d, cc2d);
			}
		}

		if (SceneManager::GetActiveScene()->IsRunning())
			s_physics2DEngineData->PhysicsWorld->SetContactListener(&g_ContactListener);
	}

	// Fixed timestep guide: 
	// Original Article: https://gafferongames.com/post/fix_your_timestep/
	// Box2d Specific: https://www.unagames.com/blog/daniele/2010/06/fixed-time-step-implementation-box2d#:~:text=If%20you%20are%20interested%20in,with%20a%20variable%20frame%2Drate.
	void Physics2DEngine::OnPhysicsUpdate(Timestep ts)
	{
		const int MAX_STEPS = 5;
		s_physics2DEngineData->Accumulator += ts;
		const int nSteps = glm::floor(s_physics2DEngineData->Accumulator / s_physics2DEngineData->Settings.PHYSICS_TIMESTEP);
		if (nSteps > 0)
			s_physics2DEngineData->Accumulator -= nSteps * s_physics2DEngineData->Settings.PHYSICS_TIMESTEP;

		s_physics2DEngineData->AccumulatorRatio = s_physics2DEngineData->Accumulator / s_physics2DEngineData->Settings.PHYSICS_TIMESTEP;

		auto view = SceneManager::GetActiveScene()->GetAllEntitiesWith<Rigidbody2DComponent>();
		const int nStepsClamped = glm::min(nSteps, MAX_STEPS);
		for (int i = 0; i < nStepsClamped; ++i)
		{
			// reset smoothing
			for (auto e : view)
			{
				Entity entity = { e, SceneManager::GetActiveScene().get() };
				auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

				b2Body* body = (b2Body*)rb2d.RuntimeBody;
				ENGINE_CORE_ASSERT(body, "Entiy has Rigidbody2DComponent, but no b2Body!");
				if (body->GetType() == b2_staticBody)
				{
					continue;
				}

				auto& transform = entity.GetComponent<TransformComponent>();
				switch (rb2d.Smoothing)
				{
				case Rigidbody2DComponent::SmoothingType::Interpolation:
				{
					auto& position = body->GetPosition();
					transform.Position = glm::vec3(position.x, position.y, transform.Position.z);
					rb2d.PreviousPosition = glm::vec2(position.x, position.y);
					transform.Rotation.z = rb2d.PreviousAngle = body->GetAngle();
					break;
				}
				case Rigidbody2DComponent::SmoothingType::Extrapolation:
				case Rigidbody2DComponent::SmoothingType::None:
				default:
				{
					auto& position = body->GetPosition();
					transform.Position = glm::vec3(position.x, position.y, transform.Position.z);
					transform.Rotation.z = body->GetAngle();
					break;
				}
				}
			}

			s_physics2DEngineData->PhysicsWorld->Step(s_physics2DEngineData->Settings.PHYSICS_TIMESTEP, s_physics2DEngineData->Settings.VELOCITY_INTERACTIONS, s_physics2DEngineData->Settings.POSITION_INTERACTIONS);
			DestroyQueuedBodiesToDestroy();
			SetPositionQueuedBodiesToPosition();
			SetRotationQueuedBodiesToRotate();
		}

		s_physics2DEngineData->PhysicsWorld->ClearForces();

		// Smooth on Physics step only for consistent behavior in regards to framerate
		if (nStepsClamped <= 0)
			return;

		// apply smoothing
		const float oneMinusRatio = 1.0f - s_physics2DEngineData->AccumulatorRatio;

		for (auto e : view)
		{
			Entity entity = { e, SceneManager::GetActiveScene().get() };
			auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

			b2Body* body = (b2Body*)rb2d.RuntimeBody;
			ENGINE_CORE_ASSERT(body, "Entiy has Rigidbody2DComponent, but no b2Body!");
			if (body->GetType() == b2_staticBody)
				continue;

			auto& transform = entity.GetComponent<TransformComponent>();
			switch (rb2d.Smoothing)
			{
				case Rigidbody2DComponent::SmoothingType::Interpolation:
				{
					auto& position = s_physics2DEngineData->AccumulatorRatio * body->GetPosition() + oneMinusRatio * b2Vec2(rb2d.PreviousPosition.x, rb2d.PreviousPosition.y);
					transform.Position = glm::vec3(position.x, position.y, transform.Position.z);
					transform.Rotation.z = s_physics2DEngineData->AccumulatorRatio * body->GetAngle() + oneMinusRatio * rb2d.PreviousAngle;
					break;
				}
				case Rigidbody2DComponent::SmoothingType::Extrapolation:
				{
					float dt = s_physics2DEngineData->AccumulatorRatio * s_physics2DEngineData->Settings.PHYSICS_TIMESTEP;
					auto& position = body->GetPosition() + dt * body->GetLinearVelocity();
					transform.Position = glm::vec3(position.x, position.y, transform.Position.z);
					transform.Rotation.z = body->GetAngle() + dt * body->GetAngularVelocity();
					break;
				}
				case Rigidbody2DComponent::SmoothingType::None:
				default:
				{
					auto& position = body->GetPosition();
					transform.Position = glm::vec3(position.x, position.y, transform.Position.z);
					transform.Rotation.z = body->GetAngle();
					break;
				}
			}
		}
	}

	void Physics2DEngine::OnPhysicsStop()
	{
		delete s_physics2DEngineData->PhysicsWorld;
		s_physics2DEngineData->PhysicsWorld = nullptr;

		delete s_physics2DEngineData;
	}

	b2Body* Physics2DEngine::CreateRigidbody(Entity entity)
	{
		auto& transform = entity.GetComponent<TransformComponent>();
		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

		b2BodyDef bodyDef;
		bodyDef.type = Utils::Rigidbody2DTypeToBox2DBodyType(rb2d.Type);
		bodyDef.position.Set(transform.Position.x, transform.Position.y);
		bodyDef.angle = transform.Rotation.z;
		bodyDef.gravityScale = rb2d.GravityScale;
		
		UUID entityID = entity.GetUUID();
		bodyDef.userData.pointer = entityID; // check if this needs to be cleared if not a pointer

		rb2d.PreviousPosition = transform.Position;
		rb2d.PreviousAngle = transform.Rotation.z;

		b2Body* body = s_physics2DEngineData->PhysicsWorld->CreateBody(&bodyDef);

		body->SetFixedRotation(rb2d.FixedRotation);

		return body;
	}

	void Physics2DEngine::DestroyBody(Entity entity)
	{
		if (!entity.HasComponent<Rigidbody2DComponent>())
			return;

		b2Body* body = (b2Body*)entity.GetComponent<Rigidbody2DComponent>().RuntimeBody;
		if (!body)
			return;

		if (s_physics2DEngineData->PhysicsWorld->IsLocked())
			s_physics2DEngineData->QueuedBodiesToDestroy.emplace_back(body);
		else
			s_physics2DEngineData->PhysicsWorld->DestroyBody(body);
	}

	void Physics2DEngine::SetRigidbodyPosition(Entity entity, glm::vec2 position)
	{
		if (!entity.HasComponent<Rigidbody2DComponent>())
			return;

		b2Body* body = (b2Body*)entity.GetComponent<Rigidbody2DComponent>().RuntimeBody;
		if (!body)
			return;

		if (s_physics2DEngineData->PhysicsWorld->IsLocked())
			s_physics2DEngineData->QueuedBodiesToPosition[body] = position;
		else
			body->SetTransform({ position.x, position.y }, body->GetAngle());
	}

	void Physics2DEngine::SetRigidbodyRotation(Entity entity, float angle)
	{
		if (!entity.HasComponent<Rigidbody2DComponent>())
			return;

		b2Body* body = (b2Body*)entity.GetComponent<Rigidbody2DComponent>().RuntimeBody;
		if (!body)
			return;

		if (s_physics2DEngineData->PhysicsWorld->IsLocked())
			s_physics2DEngineData->QueuedBodiesToRotate[body] = angle;
		else
			body->SetTransform(body->GetPosition(), angle);
	}

	b2Fixture* Physics2DEngine::CreateCollider(const TransformComponent& transform, const Rigidbody2DComponent& rb2d, const BoxCollider2DComponent& bc2d)
	{
		b2PolygonShape boxShape;
		boxShape.SetAsBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y, b2Vec2(bc2d.Offset.x, bc2d.Offset.y), 0);
		b2FixtureDef fixtureDef = CreateFixtureDef(boxShape, bc2d.Density, bc2d.Friction, bc2d.Restitution, bc2d.RestitutionThreshold, bc2d.Sensor);

		b2Body* body = (b2Body*)rb2d.RuntimeBody;
		return body->CreateFixture(&fixtureDef);
	}


	b2Fixture* Physics2DEngine::CreateCollider(const TransformComponent& transform, const Rigidbody2DComponent& rb2d, const CircleCollider2DComponent& cc2d)
	{
		b2CircleShape circleShape;
		circleShape.m_p.Set(cc2d.Offset.x, cc2d.Offset.y);
		circleShape.m_radius = transform.Scale.x * cc2d.Radius;
		b2FixtureDef fixtureDef = CreateFixtureDef(circleShape, cc2d.Density, cc2d.Friction, cc2d.Restitution, cc2d.RestitutionThreshold, cc2d.Sensor);

		b2Body* body = (b2Body*)rb2d.RuntimeBody;
		return body->CreateFixture(&fixtureDef);
	}

	void Physics2DContactListener::BeginContact(b2Contact* contact)
	{
		b2Fixture* fixtureA = contact->GetFixtureA();
		b2Fixture* fixtureB = contact->GetFixtureB();

		UUID fixtureAEntityID = (UUID)fixtureA->GetBody()->GetUserData().pointer;
		UUID fixtureBEntityID = (UUID)fixtureB->GetBody()->GetUserData().pointer;

		if (!SceneManager::GetActiveScene()->DoesEntityExist(fixtureAEntityID) && !SceneManager::GetActiveScene()->DoesEntityExist(fixtureBEntityID))
			return;

		for (b2Body* body : s_physics2DEngineData->QueuedBodiesToDestroy)
		{
			if (fixtureA->GetBody() == body || fixtureB->GetBody() == body)
			{
				return;
			}
		}

		Entity entityA = SceneManager::GetActiveScene()->GetEntityWithUUID(fixtureAEntityID);
		Entity entityB = SceneManager::GetActiveScene()->GetEntityWithUUID(fixtureBEntityID);

		if (fixtureA->IsSensor() && entityB.HasComponent<ScriptComponent>())
		{
			ScriptEngine::OnTriggerEnter2D(entityB, Physics2DContact{ fixtureAEntityID, fixtureBEntityID });
		}
		else if(fixtureB->IsSensor() && entityA.HasComponent<ScriptComponent>())
		{
			ScriptEngine::OnTriggerEnter2D(entityA, Physics2DContact{ fixtureBEntityID, fixtureAEntityID });
		}
		else
		{
			if (SceneManager::GetActiveScene()->DoesEntityExist(fixtureBEntityID) && entityB.HasComponent<ScriptComponent>())
				ScriptEngine::OnCollisionEnter2D(entityB, Physics2DContact{ fixtureAEntityID, fixtureBEntityID });
			
			if (SceneManager::GetActiveScene()->DoesEntityExist(fixtureAEntityID) && entityA.HasComponent<ScriptComponent>())
				ScriptEngine::OnCollisionEnter2D(entityA, Physics2DContact{ fixtureBEntityID, fixtureAEntityID });
		}
	}

	void Physics2DContactListener::EndContact(b2Contact* contact)
	{
		b2Fixture* fixtureA = contact->GetFixtureA();
		b2Fixture* fixtureB = contact->GetFixtureB();

		UUID fixtureAEntityID = (UUID)fixtureA->GetBody()->GetUserData().pointer;
		UUID fixtureBEntityID = (UUID)fixtureB->GetBody()->GetUserData().pointer;

		if (!SceneManager::GetActiveScene()->DoesEntityExist(fixtureAEntityID) && !SceneManager::GetActiveScene()->DoesEntityExist(fixtureBEntityID))
			return;

		for (b2Body* body : s_physics2DEngineData->QueuedBodiesToDestroy)
		{
			if (fixtureA->GetBody() == body || fixtureB->GetBody() == body)
			{
				return;
			}
		}

		Entity entityA = SceneManager::GetActiveScene()->GetEntityWithUUID(fixtureAEntityID);
		Entity entityB = SceneManager::GetActiveScene()->GetEntityWithUUID(fixtureBEntityID);

		if (fixtureA->IsSensor() && SceneManager::GetActiveScene()->DoesEntityExist(fixtureBEntityID) && entityB.HasComponent<ScriptComponent>())
		{
			ScriptEngine::OnTriggerExit2D(entityB, Physics2DContact{ fixtureAEntityID, fixtureBEntityID });
		}
		else if (fixtureB->IsSensor() && SceneManager::GetActiveScene()->DoesEntityExist(fixtureAEntityID) && entityA.HasComponent<ScriptComponent>())
		{
			ScriptEngine::OnTriggerExit2D(entityA, Physics2DContact{ fixtureBEntityID, fixtureAEntityID });
		}
		else
		{
			if (SceneManager::GetActiveScene()->DoesEntityExist(fixtureBEntityID) && entityB.HasComponent<ScriptComponent>())
				ScriptEngine::OnCollisionExit2D(entityB, Physics2DContact{ fixtureAEntityID, fixtureBEntityID });

			if (SceneManager::GetActiveScene()->DoesEntityExist(fixtureAEntityID) && entityA.HasComponent<ScriptComponent>())
				ScriptEngine::OnCollisionExit2D(entityA, Physics2DContact{ fixtureBEntityID, fixtureAEntityID });
		}
	}
}

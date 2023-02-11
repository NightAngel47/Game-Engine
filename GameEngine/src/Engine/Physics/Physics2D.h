#pragma once
#include "Engine/Scene/Components.h"
#include "Engine/Scene/Entity.h"

#include <box2d/b2_body.h>
#include <box2d/b2_world.h>
#include "box2d/b2_contact.h"

namespace Engine
{
	namespace Utils
	{
		inline b2BodyType Rigidbody2DTypeToBox2DBodyType(Rigidbody2DComponent::BodyType bodyType)
		{
			switch (bodyType)
			{
			case Rigidbody2DComponent::BodyType::Static:	return b2BodyType::b2_staticBody;
			case Rigidbody2DComponent::BodyType::Dynamic:	return b2BodyType::b2_dynamicBody;
			case Rigidbody2DComponent::BodyType::Kinematic:	return b2BodyType::b2_kinematicBody;
			}

			ENGINE_ASSERT(false, "Unknown body type!");
			return b2_staticBody;
		}

		inline Rigidbody2DComponent::BodyType Box2DBodyTypeToRigidbody2DType(b2BodyType bodyType)
		{
			switch (bodyType)
			{
			case b2BodyType::b2_staticBody:		return Rigidbody2DComponent::BodyType::Static;
			case b2BodyType::b2_dynamicBody:	return Rigidbody2DComponent::BodyType::Dynamic;
			case b2BodyType::b2_kinematicBody:	return Rigidbody2DComponent::BodyType::Kinematic;
			}

			ENGINE_ASSERT(false, "Unknown body type!");
			return Rigidbody2DComponent::BodyType::Static;
		}

		inline static std::string Rigidbody2DBodyTypeToString(Rigidbody2DComponent::BodyType bodyType)
		{
			switch (bodyType)
			{
			case Rigidbody2DComponent::BodyType::Static:	return "Static";
			case Rigidbody2DComponent::BodyType::Dynamic:	return "Dynamic";
			case Rigidbody2DComponent::BodyType::Kinematic:	return "Kinematic";
			}

			ENGINE_CORE_ASSERT(false, "Unknown body type!");
			return {};
		}

		inline static Rigidbody2DComponent::BodyType Rigidbody2DBodyTypeFromString(const std::string& bodyTypeString)
		{
			if (bodyTypeString == "Static")		return Rigidbody2DComponent::BodyType::Static;
			if (bodyTypeString == "Dynamic")	return Rigidbody2DComponent::BodyType::Dynamic;
			if (bodyTypeString == "Kinematic")	return Rigidbody2DComponent::BodyType::Kinematic;

			ENGINE_CORE_ASSERT(false, "Unknown body type!");
			return Rigidbody2DComponent::BodyType::Static;
		}

		inline static std::string Rigidbody2DSmoothingTypeToString(Rigidbody2DComponent::SmoothingType smoothingType)
		{
			switch (smoothingType)
			{
			case Rigidbody2DComponent::SmoothingType::None:				return "None";
			case Rigidbody2DComponent::SmoothingType::Interpolation:	return "Interpolation";
			case Rigidbody2DComponent::SmoothingType::Extrapolation:	return "Extrapolation";
			}

			ENGINE_CORE_ASSERT(false, "Unknown smoothing type!");
			return {};
		}

		inline static Rigidbody2DComponent::SmoothingType Rigidbody2DSmoothingTypeFromString(const std::string& smoothingTypeString)
		{
			if (smoothingTypeString == "None")			return Rigidbody2DComponent::SmoothingType::None;
			if (smoothingTypeString == "Interpolation")	return Rigidbody2DComponent::SmoothingType::Interpolation;
			if (smoothingTypeString == "Extrapolation")	return Rigidbody2DComponent::SmoothingType::Extrapolation;

			ENGINE_CORE_ASSERT(false, "Unknown smoothing type!");
			return Rigidbody2DComponent::SmoothingType::Interpolation;
		}
	}

	class Physics2DEngine
	{
	public:
		static void OnPhysicsStart(Scene* scene);
		static void OnPhysicsUpdate(Timestep ts);
		static void OnPhysicsStop();

		static b2Body* CreateRigidbody(Entity entity);
		static void DestroyBody(Entity entity);

		static b2Fixture* CreateCollider(const TransformComponent& transform, const Rigidbody2DComponent& rb2d, const BoxCollider2DComponent& bc2d);
		static b2Fixture* CreateCollider(const TransformComponent& transform, const Rigidbody2DComponent& rb2d, const CircleCollider2DComponent& cc2d);

	private:
	};

	class Physics2DContactListener : public b2ContactListener
	{
	public:
		void BeginContact(b2Contact* contact) override;
		void EndContact(b2Contact* contact) override;
	};

	struct Physics2DContact
	{
		uint64_t colliderEntityID = UUID::INVALID();
		uint64_t otherEntityID = UUID::INVALID();
	};
}

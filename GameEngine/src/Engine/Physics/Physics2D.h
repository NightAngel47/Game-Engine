#pragma once
#include "Engine/Scene/Components.h"

#include <box2d/b2_body.h>

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
	}
}

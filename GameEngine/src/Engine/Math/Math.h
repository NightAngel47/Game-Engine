#pragma once

#include <glm/glm.hpp>

namespace Engine
{
	namespace Math
	{
		bool DecomposeTransform(const glm::mat4& transform, glm::vec3& outPosition, glm::vec3& outRotation, glm::vec3& outScale);
		glm::mat4 GenRectTransform(const glm::vec3& position, const float rotation, const glm::vec2& size);
	}
}

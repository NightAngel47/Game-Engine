#pragma once

#include <glm/glm.hpp>

namespace Engine
{
	class Camera
	{
	public:
		Camera() = default;
		Camera(const glm::mat4& projection, const float width, const float height)
			: m_Projection(projection), m_ViewportWidth(width), m_ViewportHeight(height) {}

		virtual ~Camera() = default;

		const glm::mat4& GetProjection() const { return m_Projection; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() const { return m_Projection * m_ViewMatrix; }

		virtual inline void SetViewportSize(float width, float height) 
		{
			ENGINE_CORE_ASSERT(width > 0 && height > 0, "Width and/or Height is 0 or Negative");
			m_ViewportWidth = width; 
			m_ViewportHeight = height;
		}
	protected:
		glm::mat4 m_Projection = glm::mat4(1.0f);
		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);

		float m_ViewportWidth = 1280;
		float m_ViewportHeight = 720;
	};
}

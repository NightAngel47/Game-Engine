#pragma once
#include "Engine/Renderer/Camera.h"

#include <glm/glm.hpp>

namespace Engine
{
	
	class OrthographicCamera : public Camera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top);
		void SetProjection(float left, float right, float bottom, float top);

		const glm::vec3& GetPosition() const { return m_Position; }
		void SetPosition(const glm::vec3& position) { m_Position = position; RecalulateViewMatrix(); }
		
		virtual inline void SetViewportSize(float width, float height) override { Camera::SetViewportSize(width, height); RecalulateViewMatrix(); }

		float GetRotation() const { return m_Rotation; }
		void SetRotation(float rotation) { m_Rotation = rotation; RecalulateViewMatrix(); }
	private:
		void RecalulateViewMatrix();
	private:
		glm::vec3 m_Position = {0.0f, 0.0f, 0.0f};
		float m_Rotation = 0.0f;
	};
}

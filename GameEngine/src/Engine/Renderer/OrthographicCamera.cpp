#include "enginepch.h"
#include "Engine/Renderer/OrthographicCamera.h"

#include "glm/gtc/matrix_transform.hpp"

namespace Engine
{
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
	{
		ENGINE_PROFILE_FUNCTION();
		m_Projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		SetViewportSize(left + right, bottom + top);
	}

	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
	{
		ENGINE_PROFILE_FUNCTION();
		m_Projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		SetViewportSize(left + right, bottom + top);
	}

	void OrthographicCamera::RecalulateViewMatrix()
	{
		ENGINE_PROFILE_FUNCTION();
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) * 
			glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));

		m_ViewMatrix = glm::inverse(transform);
	}
}

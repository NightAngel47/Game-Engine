#include "enginepch.h"
#include "Engine/Scene/SceneCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Engine
{
	SceneCamera::SceneCamera()
	{
		RecalculateProjection();
	}

	void SceneCamera::SetViewportSize(float width, float height)
	{
		Camera::SetViewportSize(width, height);
		m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
		RecalculateProjection();
	}

	void SceneCamera::SetPerspective(float verticalFov, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Perspective;
		
		m_PerspectiveFOV = verticalFov;
		m_PerspectiveNear = nearClip;
		m_PerspectiveFar = farClip;
		
		RecalculateProjection();
	}

	void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Orthographic;

		m_OrthographicSize = size;
		m_OrthographicNear = nearClip;
		m_OrthographicFar = farClip;

		RecalculateProjection();
	}

	glm::vec3 SceneCamera::ScreenToWorldRay(glm::vec2 screenPos)
	{
		float clipX = (2.0f * screenPos.x) / m_ViewportWidth - 1.0f;
		float clipY = 1.0f - (2.0f * screenPos.y) / m_ViewportHeight;

		glm::vec4 ray_eye = glm::inverse(m_Projection) * glm::vec4(clipX, clipY, 1.0f, 1.0f);
		glm::vec3 ray_wor = glm::inverse(m_ViewMatrix) * ray_eye;

		return ray_wor;
	}

	void SceneCamera::RecalculateProjection()
	{
		if(m_ProjectionType == ProjectionType::Perspective)
		{
			m_Projection = glm::perspective(glm::radians(m_PerspectiveFOV), m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
		}
		else
		{
			float orthoLeft = -m_OrthographicSize * m_AspectRatio * 0.5f;
			float orthoRight = m_OrthographicSize * m_AspectRatio * 0.5f;
			float orthoBottom = -m_OrthographicSize * 0.5f;
			float orthoTop = m_OrthographicSize * 0.5f;

			m_Projection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, m_OrthographicNear, m_OrthographicFar);
		}
	}
}

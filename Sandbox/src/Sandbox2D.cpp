#include "Sandbox2D.h"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f, true)
{
	ENGINE_PROFILE_FUNCTION();
}

void Sandbox2D::OnAttach()
{
	ENGINE_PROFILE_FUNCTION();
	
	m_CheckerboardTexture = Engine::Texture2D::Create("assets/textures/Checkerboard.png");
	m_TempleTexture = Engine::Texture2D::Create("assets/textures/temple.png");
	m_ShipTexture = Engine::Texture2D::Create("assets/textures/shipGreen_manned.png");
}

void Sandbox2D::OnDetach()
{
	ENGINE_PROFILE_FUNCTION();
	
}

void Sandbox2D::OnUpdate(Engine::Timestep ts)
{
	ENGINE_PROFILE_FUNCTION();
	
	// Update
	m_CameraController.OnUpdate(ts);

	m_SmallSquareRotation += m_RotationSpeed * ts;
	
	// Render
	{
		ENGINE_PROFILE_SCOPE("Renderer Prep");
		Engine::RenderCommand::SetClearColor(m_ClearColor);
		Engine::RenderCommand::Clear();
	}

	{
		ENGINE_PROFILE_SCOPE("Renderer Draw");
		Engine::Renderer2D::BeginScene(m_CameraController.GetCamera());

		Engine::Renderer2D::DrawQuad({-1.0f, 1.0f}, 0.0f, {1.0f, 1.0f}, m_SquareColor);
		Engine::Renderer2D::DrawQuad({1.0f, 1.0f}, 0.0f, {0.5f, 0.5f}, m_SmallSquareColor);
		Engine::Renderer2D::DrawQuad({0.0f, -1.0f}, 0.0f, {2.5f, 0.5f}, m_RectColor);
		
		Engine::Renderer2D::DrawQuad({-1.0f, -1.0f, -0.1f}, 0.0f, {1.0f, 1.0f}, m_CheckerboardTexture, 3.333f);
		Engine::Renderer2D::DrawQuad({-32.0f, -32.0f, -0.2f}, 0.0f, {64.0f, 64.0f}, m_CheckerboardTexture, 10.0f);
		Engine::Renderer2D::DrawQuad({0.0f, 0.0f, -0.1f}, 0.0f, {22.3f, 13.1f}, m_TempleTexture, 1.0f, {0.65f, 0.0f, 0.65f, 1.0f});
		Engine::Renderer2D::DrawQuad({0.0f, 0.0f}, 0.0f, {1.24f, 1.23f}, m_ShipTexture);
		
		Engine::Renderer2D::EndScene();
	}
}

void Sandbox2D::OnImGuiRender()
{
	ENGINE_PROFILE_FUNCTION();
	
	ImGui::Begin("Settings");
	ImGui::ColorEdit3("Clear Color", glm::value_ptr(m_ClearColor));
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::ColorEdit4("Small Square Color", glm::value_ptr(m_SmallSquareColor));
	ImGui::ColorEdit4("Rectangle Color", glm::value_ptr(m_RectColor));
	ImGui::End();
}

void Sandbox2D::OnEvent(Engine::Event& e)
{
	ENGINE_PROFILE_FUNCTION();
		
	m_CameraController.OnEvent(e);
}

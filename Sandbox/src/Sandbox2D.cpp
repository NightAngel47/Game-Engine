#include "Sandbox2D.h"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f, false)
{
	ENGINE_PROFILE_FUNCTION();
}

void Sandbox2D::OnAttach()
{
	ENGINE_PROFILE_FUNCTION();
	
	m_CheckerboardTexture = Engine::Texture2D::Create("assets/textures/Checkerboard.png");
	m_TempleTexture = Engine::Texture2D::Create("assets/textures/temple.png");
	m_ShipTexture = Engine::Texture2D::Create("assets/textures/shipGreen_manned.png");
	m_SpriteSheet = Engine::Texture2D::Create("assets/game/textures/RPGpack_sheet_2X.png");

	m_Particle.ColorBegin = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
	m_Particle.ColorEnd = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
	m_Particle.SizeBegin = 0.5f, m_Particle.SizeVariation = 0.3f, m_Particle.SizeEnd = 0.0f;
	m_Particle.LifeTime = 5.0f;
	m_Particle.Velocity = { 0.0f, 0.0f };
	m_Particle.VelocityVariation = { 3.0f, 1.0f };
	m_Particle.Position = { 0.0f, 0.0f };
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
	Engine::Renderer2D::ResetStats();
	{
		ENGINE_PROFILE_SCOPE("Renderer Prep");
		
		Engine::RenderCommand::SetClearColor(m_ClearColor);
		Engine::RenderCommand::Clear();
	}

	{
		ENGINE_PROFILE_SCOPE("Renderer Draw");
		/*
		Engine::Renderer2D::BeginScene(m_CameraController.GetCamera());

		Engine::Renderer2D::DrawQuad({-1.0f, 1.0f}, 0.0f, {1.0f, 1.0f}, m_SquareColor);
		Engine::Renderer2D::DrawQuad({1.0f, 1.0f}, m_SmallSquareRotation, {0.5f, 0.5f}, m_SmallSquareColor);
		Engine::Renderer2D::DrawQuad({0.0f, -1.0f}, 35.0f, {2.5f, 0.5f}, m_RectColor);
		
		Engine::Renderer2D::DrawQuad({-1.0f, -1.0f, -0.1f}, 5.0f, {1.0f, 1.0f}, m_CheckerboardTexture, 3.333f);
		Engine::Renderer2D::DrawQuad({0.0f, 0.0f, -0.2f}, 0.0f, {64.0f, 64.0f}, m_CheckerboardTexture, 10.0f);
		Engine::Renderer2D::DrawQuad({0.0f, 0.0f, -0.1f}, 0.0f, {22.3f, 13.1f}, m_TempleTexture, 1.0f, {0.65f, 0.0f, 0.65f, 1.0f});
		Engine::Renderer2D::DrawQuad({0.0f, 0.0f}, 0.0f, {1.24f, 1.23f}, m_ShipTexture);
		
		Engine::Renderer2D::EndScene();
		
		Engine::Renderer2D::BeginScene(m_CameraController.GetCamera());

		for (float y = -15.0f; y < -5.0f; y += 0.5f)
		{
			for (float x = -15.0f; x < -5.0f; x += 0.5f)
			{
				glm::vec4 color = {(x + 15.0f) / 10.0f, 0.4f, (y + 15.0f) / 10.0f, 0.75f};
				Engine::Renderer2D::DrawQuad({x, y}, 0.0f, {0.45f, 0.45f}, color);
			}
		}
		
		Engine::Renderer2D::EndScene();
		*/
		
		Engine::Renderer2D::BeginScene(m_CameraController.GetCamera());

		Engine::Renderer2D::DrawQuad({0.0f, 0.0f}, 0.0f, {1.0f, 1.0f}, m_SpriteSheet);
		
		Engine::Renderer2D::EndScene();
	}

	if (Engine::Input::IsMouseButtonPressed(ENGINE_MOUSE_BUTTON_LEFT))
	{
		auto [x, y] = Engine::Input::GetMousePosition();
		auto width = Engine::Application::Get().GetWindow().GetWidth();
		auto height = Engine::Application::Get().GetWindow().GetHeight();

		auto bounds = m_CameraController.GetBounds();
		auto pos = m_CameraController.GetCamera().GetPosition();
		x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
		y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
		m_Particle.Position = { x + pos.x, y + pos.y };
		for (int i = 0; i < 5; i++)
			m_ParticleSystem.Emit(m_Particle);
	}

	m_ParticleSystem.OnUpdate(ts);
	m_ParticleSystem.OnRender(m_CameraController.GetCamera());
}

void Sandbox2D::OnImGuiRender()
{
	ENGINE_PROFILE_FUNCTION();
	
	ImGui::Begin("Settings");

	auto stats = Engine::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	ImGui::Text("Quad Count: %d", stats.QuadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
	
	ImGui::Text("");
	
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

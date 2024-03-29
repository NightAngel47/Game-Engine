#include "Sandbox2D.h"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>

#include "Engine/Debug/Instrumentor.h"


static const uint32_t s_MapWidth = 24;
static const char* s_MapTiles = 
"WWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWDDDDDDWWWWWWWWWWW"
"WWWWWDDDDDDDDDDDWWWWWWWW"
"WWWWDDDDDDDDDDDDDDDWWWWW"
"WWWDDDDDDDDDDDDDDDDDDWWW"
"WWDDDDWWWDDDDDDDDDDDDWWW"
"WDDDDDWWWDDDDDDDDDDDDDWW"
"WWDDDDDDDDDDDDDDDDDDDWWW"
"WWWWDDDDDDDDDDDDDDDDWWWW"
"WWWWWDDDDDDDDDDDDDDWWWWW"
"WWWWWWDDDDDDDDDDDWWWWWWW"
"WWWWWWWDDDDDDDDDWWWWWWWW"
"WWWWWWWWWWDDDDWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWW"
;

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

	m_TextureBarrel = Engine::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 8, 2 }, { 128, 128 });
	m_TextureStairs = Engine::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 7, 6 }, { 128, 128 });
	m_TextureTree = Engine::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 2, 1 }, { 128, 128 }, { 1, 2 });

	m_MapWidth = s_MapWidth;
	m_MapHeight = strlen(s_MapTiles) / s_MapWidth;

	s_TextureMap['D'] = Engine::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 6, 11 }, { 128, 128 });
	s_TextureMap['W'] = Engine::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 11, 11 }, { 128, 128 });

	m_Particle.ColorBegin = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
	m_Particle.ColorEnd = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
	m_Particle.SizeBegin = 0.5f, m_Particle.SizeVariation = 0.3f, m_Particle.SizeEnd = 0.0f;
	m_Particle.LifeTime = 5.0f;
	m_Particle.Velocity = { 0.0f, 0.0f };
	m_Particle.VelocityVariation = { 3.0f, 1.0f };
	m_Particle.Position = { 0.0f, 0.0f };

	m_CameraController.SetZoomLevel(5.0f);
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
		
		Engine::Renderer2D::BeginScene(m_CameraController.GetCamera(), glm::mat4(1.0f));

		for (uint32_t y = 0; y < m_MapHeight; y++)
		{
			for (uint32_t x = 0; x < m_MapWidth; x++)
			{
				char tileType = s_MapTiles[x + y * m_MapWidth];
				Engine::Ref<Engine::SubTexture2D> texture;
				if (s_TextureMap.find(tileType) != s_TextureMap.end())
					texture = s_TextureMap[tileType];
				else
					texture = m_TextureBarrel;
		
				Engine::Renderer2D::DrawQuad({x - m_MapWidth / 2.0f, m_MapHeight - y - m_MapHeight / 2.0f}, 0.0f, {1.0f, 1.0f}, texture);
			}
		}

		// Engine::Renderer2D::DrawQuad({0.0f, 0.0f}, 0.0f, {1.0f, 1.0f}, m_TextureStairs);
		// Engine::Renderer2D::DrawQuad({1.0f, 0.0f}, 0.0f, {1.0f, 1.0f}, m_TextureBarrel);
		// Engine::Renderer2D::DrawQuad({-1.0f, 0.5f}, 0.0f, {1.0f, 2.0f}, m_TextureTree);
		
		Engine::Renderer2D::EndScene();
	}

	if (Engine::Input::IsMouseButtonPressed(Engine::Mouse::ButtonLeft))
	{
		auto mousePos = Engine::Input::GetMousePosition();
		auto width = Engine::Application::Get().GetWindow().GetWidth();
		auto height = Engine::Application::Get().GetWindow().GetHeight();

		auto bounds = m_CameraController.GetBounds();
		auto pos = m_CameraController.GetCamera().GetPosition();
		mousePos.x = (mousePos.x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
		mousePos.y = bounds.GetHeight() * 0.5f - (mousePos.y / height) * bounds.GetHeight();
		m_Particle.Position = { mousePos.x + pos.x, mousePos.y + pos.y };
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

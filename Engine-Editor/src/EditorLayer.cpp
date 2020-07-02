#include "EditorLayer.h"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>

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

namespace Engine
{
	EditorLayer::EditorLayer()
		: Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f, false)
	{
		ENGINE_PROFILE_FUNCTION();
	}

	void EditorLayer::OnAttach()
	{
		ENGINE_PROFILE_FUNCTION();

		Engine::FramebufferSpecification fbSpec;
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Framebuffer = Engine::Framebuffer::Create(fbSpec);
		
		m_CheckerboardTexture = Engine::Texture2D::Create("assets/textures/Checkerboard.png");
		m_TempleTexture = Engine::Texture2D::Create("assets/textures/temple.png");
		m_ShipTexture = Engine::Texture2D::Create("assets/textures/shipGreen_manned.png");
		m_SpriteSheet = Engine::Texture2D::Create("assets/game/textures/RPGpack_sheet_2X.png");

		m_TextureBarrel = Engine::SubTexture2D::CreateFromCoords(m_SpriteSheet, {8, 2}, {128, 128});
		m_TextureStairs = Engine::SubTexture2D::CreateFromCoords(m_SpriteSheet, {7, 6}, {128, 128});
		m_TextureTree = Engine::SubTexture2D::CreateFromCoords(m_SpriteSheet, {2, 1}, {128, 128}, {1, 2});

		m_MapWidth = s_MapWidth;
		m_MapHeight = strlen(s_MapTiles) / s_MapWidth;
		
		s_TextureMap['D'] = Engine::SubTexture2D::CreateFromCoords(m_SpriteSheet, {6, 11}, {128, 128});
		s_TextureMap['W'] = Engine::SubTexture2D::CreateFromCoords(m_SpriteSheet, {11, 11}, {128, 128});

		m_CameraController.SetZoomLevel(5.0f);
	}

	void EditorLayer::OnDetach()
	{
		ENGINE_PROFILE_FUNCTION();
		
	}

	void EditorLayer::OnUpdate(Engine::Timestep ts)
	{
		ENGINE_PROFILE_FUNCTION();
		
		// Update
		if (m_ViewportFocused)
			m_CameraController.OnUpdate(ts);

		m_SmallSquareRotation += m_RotationSpeed * ts;
		
		// Render
		Engine::Renderer2D::ResetStats();
		{
			ENGINE_PROFILE_SCOPE("Renderer Prep");

			m_Framebuffer->Bind();
			
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

		m_Framebuffer->Unbind();
	}

	void EditorLayer::OnImGuiRender()
	{
		ENGINE_PROFILE_FUNCTION();

		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
	    bool opt_fullscreen = opt_fullscreen_persistant;
	    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	    // because it would be confusing to have two docking targets within each others.
	    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	    if (opt_fullscreen)
	    {
	        ImGuiViewport* viewport = ImGui::GetMainViewport();
	        ImGui::SetNextWindowPos(viewport->GetWorkPos());
	        ImGui::SetNextWindowSize(viewport->GetWorkSize());
	        ImGui::SetNextWindowViewport(viewport->ID);
	        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	    }

	    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
	    // and handle the pass-thru hole, so we ask Begin() to not render a background.
	    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
	        window_flags |= ImGuiWindowFlags_NoBackground;

	    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
	    // all active windows docked into it will lose their parent and become undocked.
	    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
	    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	    ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
	    ImGui::PopStyleVar();

	    if (opt_fullscreen)
	        ImGui::PopStyleVar(2);

	    // DockSpace
	    ImGuiIO& io = ImGui::GetIO();
	    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	    {
	        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	    }

	    if (ImGui::BeginMenuBar())
	    {
	        if (ImGui::BeginMenu("File"))
	        {
	            // Disabling fullscreen would allow the window to be moved to the front of other windows,
	            // which we can't undo at the moment without finer window depth/z control.
	            //ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

	            if (ImGui::MenuItem("Exit")) Engine::Application::Get().Close();
	            ImGui::EndMenu();
	        }

	        ImGui::EndMenuBar();
	    }

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

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
		ImGui::Begin("Viewport");

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);
		
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		if (m_ViewportSize != *((glm::vec2*)&viewportPanelSize) && viewportPanelSize.x > 0 && viewportPanelSize.y > 0)
		{
			m_Framebuffer->Resize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
			m_ViewportSize = {viewportPanelSize.x, viewportPanelSize.y};

			m_CameraController.OnResize(viewportPanelSize.x, viewportPanelSize.y);
		}
		uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image((void*)textureID, ImVec2{m_ViewportSize.x, m_ViewportSize.y}, ImVec2{0, 1}, ImVec2{1, 0});
		ImGui::End();
		ImGui::PopStyleVar();
		
	    ImGui::End();
	}

	void EditorLayer::OnEvent(Engine::Event& e)
	{
		ENGINE_PROFILE_FUNCTION();
		
		m_CameraController.OnEvent(e);
	}
	
}
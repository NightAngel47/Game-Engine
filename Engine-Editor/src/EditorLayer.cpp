#include "EditorLayer.h"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>

#include "Engine/Scene/SceneSerializer.h"
#include "Engine/Utils/PlatformUtils.h"

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

		FramebufferSpecification fbSpec;
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Framebuffer = Framebuffer::Create(fbSpec);

		m_ActiveScene = CreateRef<Scene>();

#if 0
		// Entity
		Entity square = m_ActiveScene->CreateEntity("Square");
		square.AddComponent<SpriteRendererComponent>(glm::vec4{0.0f, 1.0f, 0.0f, 1.0f});
		
		Entity anotherSquare = m_ActiveScene->CreateEntity("Another Square");
		anotherSquare.AddComponent<SpriteRendererComponent>(glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});

		m_SquareEntity = square;

		m_CameraEntity = m_ActiveScene->CreateEntity("Camera Entity");
		m_CameraEntity.AddComponent<CameraComponent>();

		m_SecondCamera = m_ActiveScene->CreateEntity("Second Camera");
		auto& cc = m_SecondCamera.AddComponent<CameraComponent>();
		cc.Primary = false;

		class CameraController : public ScriptableEntity
		{	
		public:
			void OnCreate()
			{
				auto& position = GetComponent<TransformComponent>().Position;
				position.x = rand() % 10 - 5.0f;
			}

			void OnDestroy()
			{
				
			}

			void OnUpdate(Timestep ts)
			{
				auto& position = GetComponent<TransformComponent>().Position;
				float speed = 5.0f;
				
				if (Input::IsKeyPressed(KeyCode::A))
					position.x -= speed * ts;
				if (Input::IsKeyPressed(KeyCode::D))
					position.x += speed * ts;
				if (Input::IsKeyPressed(KeyCode::W))
					position.y += speed * ts;
				if (Input::IsKeyPressed(KeyCode::S))
					position.y -= speed * ts;
			}
		};

		m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
		m_SecondCamera.AddComponent<NativeScriptComponent>().Bind<CameraController>();

#endif
		
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnDetach()
	{
		ENGINE_PROFILE_FUNCTION();
		
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		ENGINE_PROFILE_FUNCTION();

		// Resize
		if (FramebufferSpecification spec = m_Framebuffer->GetSpecification(); 
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && 
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);

			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}
		
		// Update
		if (m_ViewportFocused)
			m_CameraController.OnUpdate(ts);
		
		// Render
		Renderer2D::ResetStats();
		m_Framebuffer->Bind();		
		RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
		RenderCommand::Clear();

		// Update Scene
		m_ActiveScene->OnUpdate(ts);

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
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		minWinSizeX = 370.0f;
	    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	    {
	        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	    }
		
		style.WindowMinSize.x = minWinSizeX;

	    if (ImGui::BeginMenuBar())
	    {
	        if (ImGui::BeginMenu("File"))
	        {
	            // Disabling fullscreen would allow the window to be moved to the front of other windows,
	            // which we can't undo at the moment without finer window depth/z control.
	            //ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

	        	if (ImGui::MenuItem("New", "Ctrl+N)"))
					NewScene();

	        	if (ImGui::MenuItem("Open...", "Ctrl+O)"))
	        		OpenScene();

	            if (ImGui::MenuItem("Save", "Ctrl+S)"))
					SaveScene();

	            if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S)"))
					SaveSceneAs();

	            if (ImGui::MenuItem("Exit"))
					Application::Get().Close();
	        	
	            ImGui::EndMenu();
	        }

	        ImGui::EndMenuBar();
	    }

		m_SceneHierarchyPanel.OnImGuiRender();

		ImGui::Begin("Stats");
		
		auto stats = Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quad Count: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
		ImGui::Begin("Viewport");

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);
		
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = {viewportPanelSize.x, viewportPanelSize.y};
		
		uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image((void*)textureID, ImVec2{m_ViewportSize.x, m_ViewportSize.y}, ImVec2{0, 1}, ImVec2{1, 0});
		ImGui::End();
		ImGui::PopStyleVar();
		
	    ImGui::End();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		ENGINE_PROFILE_FUNCTION();
		
		m_CameraController.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(ENGINE_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		// Shortcuts
		if (e.GetRepeatCount() > 0) return false;
		
		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
		switch (e.GetKeyCode())
		{
			case Key::N:
			{
				if(control)
					NewScene();
		
				break;
			}
			case Key::O:
			{
				if(control)
					OpenScene();
		
				break;
			}
			case Key::S:
			{
				if(control && shift)
					SaveSceneAs();
				else if(control && !shift)
					SaveScene();
		
				break;
			}
		}
	}

	void EditorLayer::NewScene()
	{
		m_ActiveScene = CreateRef<Scene>();
        m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x,(uint32_t)m_ViewportSize.y);
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);

		m_SceneFilePath = {};
	}

	void EditorLayer::OpenScene()
	{
		std::string filepath = FileDialogs::OpenFile("Game Scene (*.scene)\0*.scene\0");
		if (!filepath.empty())
		{
			NewScene();
			
			m_SceneFilePath = filepath;
			SceneSerializer serializer(m_ActiveScene);
			serializer.Deserialize(filepath);
		}	
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("Game Scene (*.scene)\0*.scene\0");
		
		m_SceneFilePath = filepath;
		SceneSerializer serializer(m_ActiveScene);
		serializer.Serialize(filepath);
	}

	void EditorLayer::SaveScene()
	{
		if (m_SceneFilePath.empty())
		{
			SaveSceneAs();
		}
		else
		{
			SceneSerializer serializer(m_ActiveScene);
			serializer.Serialize(m_SceneFilePath);
		}
	}
}

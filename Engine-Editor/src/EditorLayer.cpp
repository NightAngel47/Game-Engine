#include "EditorLayer.h"

#include "Engine/Renderer/Font.h"

#include <imgui/imgui.h>
#include <ImGuizmo/ImGuizmo.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace Engine
{
	static Ref<Font> s_Font;

	void EditorLayer::OnAttach()
	{
		s_Font = Font::GetDefault();

		m_IconPlay = Texture2D::Create("Resources/Icons/PlayButton.png");
		m_IconPause = Texture2D::Create("Resources/Icons/PauseButton.png");
		m_IconStep = Texture2D::Create("Resources/Icons/StepButton.png");
		m_IconSimulate = Texture2D::Create("Resources/Icons/SimulateButton.png");
		m_IconStop = Texture2D::Create("Resources/Icons/StopButton.png");

		FramebufferSpecification fbSpec;
		fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Framebuffer = Framebuffer::Create(fbSpec);

		m_EditorScene = CreateRef<Scene>();
		m_ActiveScene = m_EditorScene;

		auto commandLineArgs = Application::Get().GetSpecification().CommandLineArgs;
		if (commandLineArgs.Count > 1)
		{
			auto projectFilePath = commandLineArgs[1];
			OpenProject(projectFilePath);
		}
		else
		{
			// TODO make project UI (new, open, etc)
			// NewProject();

			if (!OpenProject())
			{
				ENGINE_CORE_ERROR("Didn't load valid project, closing editor.");
				Application::Get().Close();
			}
		}

		m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);
		
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnDetach()
	{
		ENGINE_CORE_WARN("Not Implemented!");
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		m_FrameTime = ts.GetMilliseconds();

		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		
		// Resize
		if (FramebufferSpecification spec = m_Framebuffer->GetSpecification(); 
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && 
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
		}
		
		// Render
		Renderer2D::ResetStats();
		m_Framebuffer->Bind();		
		RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
		RenderCommand::Clear();

		// Clear our entity ID attachment to -1
		m_Framebuffer->ClearAttachment(1, -1);

		// Update Scene
		switch (m_SceneState)
		{
			case SceneState::Edit:
			{
				// Update
				m_EditorCamera.OnUpdate(ts);

				m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);
				break;
			}
			case SceneState::Play:
			{
				m_ActiveScene->OnUpdateRuntime(ts);
				break;
			}
			case SceneState::Simulate:
				m_EditorCamera.OnUpdate(ts);

				m_ActiveScene->OnUpdateSimulation(ts, m_EditorCamera);
				break;
			default:
				break;
		}

		MousePicking();

		// Overlay Rendering
		OnOverlayRender();

		m_Framebuffer->Unbind();
	}

	void EditorLayer::OnImGuiRender()
	{
		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
	    bool opt_fullscreen = opt_fullscreen_persistant;
	    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	    if (opt_fullscreen)
	    {
	        ImGuiViewport* viewport = ImGui::GetMainViewport();
	        ImGui::SetNextWindowPos(viewport->WorkPos);
	        ImGui::SetNextWindowSize(viewport->WorkSize);
	        ImGui::SetNextWindowViewport(viewport->ID);
	        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	    }

	    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
	        window_flags |= ImGuiWindowFlags_NoBackground;

	    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	    ImGui::Begin("DockSpace", &dockspaceOpen, window_flags);
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
	        	if (ImGui::MenuItem("Open Project...", "Ctrl+O)"))
					OpenProject();

				ImGui::Separator();

				if (ImGui::MenuItem("New Scene", "Ctrl+N)"))
					NewScene();

	            if (ImGui::MenuItem("Save Scene", "Ctrl+S)"))
					SaveScene();

	            if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S)"))
					SaveSceneAs();

	            if (ImGui::MenuItem("Exit"))
					Application::Get().Close();
	        	
	            ImGui::EndMenu();
	        }
			
			if (ImGui::BeginMenu("Script"))
	        {

				if (ImGui::MenuItem("Reload Assembly", "Crtl+R"))
				{
					ScriptEngine::ReloadAssembly();
					// TODO prompt user to save any changes before reloading scene
					OpenScene(m_EditorScenePath); // reload scene to match new assembly state
				}

	            ImGui::EndMenu();
	        }

	        ImGui::EndMenuBar();
	    }

		m_SceneHierarchyPanel.OnImGuiRender();
		m_ContentBrowserPanel->OnImGuiRender();

		ImGui::Begin("Stats");

		std::string name = "None";
		if (m_HoveredEntityID.IsValid())
		{
			Entity hoveredEntity = m_ActiveScene->GetEntityWithUUID(m_HoveredEntityID);
			name = hoveredEntity.GetComponent<TagComponent>().Tag;
		}
		ImGui::Text("Hovered Entity: %s", name.c_str());
		
		ImGui::Text("Frametime: %fms", m_FrameTime);

		auto stats = Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quad Count: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

		ImGui::End();

		ImGui::Begin("Settings");

		ImGui::Checkbox("Show physics colliders", &m_ShowPhysicsColliders);

		// testing font
		Ref<Texture2D> fontTexture = s_Font->GetAtlasTexture();
		ImGui::Image((ImTextureID)fontTexture->GetRendererID(), { (float)fontTexture->GetWidth(), (float)fontTexture->GetHeight() }, {0, 1}, {1, 0});

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
		ImGui::Begin("Viewport");
		
		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };
		
		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = {viewportPanelSize.x, viewportPanelSize.y};
		
		uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image((void*)(uint64_t)textureID, ImVec2{m_ViewportSize.x, m_ViewportSize.y}, ImVec2{0, 1}, ImVec2{1, 0});

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				const wchar_t* fileExtension = std::wcsrchr(path, '.');
				
				if (fileExtension)
				{
					if (std::wcscmp(fileExtension, L".scene") == 0)
					{
						OpenScene(path);
					}
					else
					{
						std::wstring ws(fileExtension);
						ENGINE_CORE_WARN("File type is not supported by drag and drop in the Viewport: " + std::string(ws.begin(), ws.end()));
					}
				}
				else
				{
					std::wstring ws(path);
					ENGINE_CORE_WARN("Dragged item is either not a file or not supported by drag and drop in the Viewport: " + std::string(ws.begin(), ws.end()));
				}
			}
			
			ImGui::EndDragDropTarget();
		}
		
		// Gizmos
		if(m_SceneHierarchyPanel.IsSelectedEntityValid() && m_GizmoType != -1)
		{
			Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			
			ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

			// Editor Camera
			const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
			glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();
			

			// Snapping
			bool snap = Input::IsKeyPressed(Key::LeftControl);
			float snapValue = 0.5f; // Snap to 0.5m for position/scale
			// Snap to 45 degrees for rotation
			if(m_GizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f;

			float snapValues[3] {snapValue, snapValue, snapValue};

			// Entity transform
			glm::mat4 transformWorld = selectedEntity.GetWorldTransform();
			glm::mat4 transformDelta{ 0.0f };

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), 
				(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::WORLD, glm::value_ptr(transformWorld),
				glm::value_ptr(transformDelta), snap ? snapValues : nullptr);

			if(ImGuizmo::IsUsing())
			{
				m_IsGizmoInUse = true;
				glm::vec3 deltaPosition{ 0.0f, 0.0f, 0.0f }, deltaRotation{ 0.0f, 0.0f, 0.0f }, deltaScale{ 0.0f, 0.0f, 0.0f };
				Math::DecomposeTransform(transformDelta, deltaPosition, deltaRotation, deltaScale);

				auto& transformComponent = selectedEntity.GetComponent<TransformComponent>();
				switch (m_GizmoType)
				{
				case ImGuizmo::OPERATION::TRANSLATE:
					transformComponent.Position += deltaPosition;
					break;
				case ImGuizmo::OPERATION::ROTATE:
					transformComponent.Rotation += deltaRotation;
					break;
				case ImGuizmo::OPERATION::SCALE:
					transformComponent.Scale *= deltaScale;
					break;
				default:
					break;
				}
			}
			else
			{
				m_IsGizmoInUse = false;
			}
		}
		
		ImGui::End();
		ImGui::PopStyleVar();

		UI_Toolbar();
		
	    ImGui::End();
	}

	void EditorLayer::UI_Toolbar()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		auto& colors = ImGui::GetStyle().Colors;
		const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
		const auto& buttonActive = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		float size = ImGui::GetWindowHeight() - 8.0f;
		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));

		// play button
		if (m_SceneState != SceneState::Simulate)
		{
			Ref<Texture2D> icon = m_SceneState == SceneState::Edit ? m_IconPlay : m_IconStop;
			if (ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
			{
				if (m_SceneState == SceneState::Play)
					OnSceneStop();
				else
					OnScenePlay();
			}
		}

		// simulate button
		if (m_SceneState != SceneState::Play)
		{
			if (m_SceneState == SceneState::Edit)
				ImGui::SameLine();

			Ref<Texture2D> icon = m_SceneState == SceneState::Edit ? m_IconSimulate : m_IconStop;
			if (ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
			{
				if (m_SceneState == SceneState::Simulate)
					OnSceneStop();
				else
					OnSceneSimulate();
			}
		}

		if (m_SceneState != SceneState::Edit)
		{
			// pause button
			ImGui::SameLine();
			{
				Ref<Texture2D> icon = m_IconPause;
				ImVec4 tintColor = m_ActiveScene->IsPaused() ? ImVec4(0.25f, 0.25f, 1, 1) : ImVec4(1, 1, 1, 1);
				if (ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0), tintColor))
				{
					m_ActiveScene->SetPaused(!m_ActiveScene->IsPaused());
				}
			}
			
			// step button
			if (m_ActiveScene->IsPaused())
			{
				ImGui::SameLine();
				{
					Ref<Texture2D> icon = m_IconStep;
					if (ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
					{
						m_ActiveScene->Step();
					}
				}
			}
		}

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_EditorCamera.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(ENGINE_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(ENGINE_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		// Shortcuts
		if (e.IsRepeat()) return false;
		
		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
		switch (e.GetKeyCode())
		{
			case Key::N:
			{
				if (control)
					NewScene();
		
				break;
			}
			case Key::O:
			{
				if (control)
					OpenProject();
		
				break;
			}
			case Key::S:
			{
				if (control && shift)
					SaveSceneAs();
				else if (control && !shift)
					SaveScene();
		
				break;
			}

			// Gizmos
			case Key::Q:
			{
				if (!m_IsGizmoInUse)
					m_GizmoType = -1;
				break;
			}
			case Key::W:
			{
				if (!m_IsGizmoInUse)
					m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
				break;
			}
			case Key::E:
			{
				if (!m_IsGizmoInUse)
					m_GizmoType = ImGuizmo::OPERATION::ROTATE;
				break;
			}
			case Key::R:
			{
				if (control)
				{
					ScriptEngine::ReloadAssembly();
				}
				else
				{
					if (!m_IsGizmoInUse)
						m_GizmoType = ImGuizmo::OPERATION::SCALE;
				}
				break;
			}

			// Other
			case Key::D:
			{
				if (control)
					OnDuplicateEntity();

				break;
			}
			case Key::Delete:
			{
				if (Application::Get().GetImGuiLayer()->GetActiveWidgetID() == 0)
				{
					if (m_SceneHierarchyPanel.IsSelectedEntityValid())
					{
						Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
						m_SceneHierarchyPanel.SetSelectedEntity(UUID::INVALID());
						m_EditorScene->DestroyEntity(selectedEntity);
					}
				}

				break;
			}
		}

		return false;
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if(e.GetMouseButton() == Mouse::ButtonLeft)
		{
			if(m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt))
			{
				m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntityID);
			}
		}
		
		return false;
	}

	void EditorLayer::OnOverlayRender()
	{
		if (m_SceneState == SceneState::Play)
		{
			Entity camera = m_ActiveScene->GetPrimaryCameraEntity();
			if (!camera) return;

			Renderer2D::BeginScene(camera.GetComponent<CameraComponent>().Camera, camera.GetWorldTransform());
		}
		else
		{
			Renderer2D::BeginScene(m_EditorCamera);
		}

		if (m_ShowPhysicsColliders)
		{
			{ // Visualize Box Collider 2D
				auto view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, BoxCollider2DComponent>();

				for (auto entity : view)
				{
					auto [tc, bc2d] = view.get<TransformComponent, BoxCollider2DComponent>(entity);

					glm::vec3 position = tc.Position + glm::vec3(glm::rotate(bc2d.Offset, tc.Rotation.z), 0.001f);
					glm::vec3 scale = tc.Scale * glm::vec3(bc2d.Size * 2.0f, 1.0f);

					glm::mat4 transform = Math::GenTransform(position, tc.Rotation.z, scale);

					Renderer2D::DrawRect(transform, glm::vec4(0, 1, 0, 1));
				}
			}

			{ // Visualize Circle Collider 2D
				auto view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, CircleCollider2DComponent>();

				for (auto entity : view)
				{
					auto [tc, cc2d] = view.get<TransformComponent, CircleCollider2DComponent>(entity);

					glm::vec3 position = tc.Position + glm::vec3(glm::rotate(cc2d.Offset, tc.Rotation.z), 0.001f);
					glm::vec3 scale = tc.Scale * glm::vec3(cc2d.Radius * 2.0f);

					glm::mat4 transform = Math::GenTransform(position, 0, scale);

					Renderer2D::DrawCircle(transform, glm::vec4(0, 1, 0, 1), 0.05f);
				}
			}
		}

		// Draw selected entity outline
		if (m_SceneHierarchyPanel.IsSelectedEntityValid())
		{
			Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
			Renderer2D::SetLineWidth(4.0f);
			Renderer2D::DrawRect(selectedEntity.GetWorldTransform(), glm::vec4(1, 0, 0.5f, 1));
		}
		else
		{
			Renderer2D::SetLineWidth(2.0f);
		}

		Renderer2D::EndScene();
	}

	void EditorLayer::NewProject()
	{
		Project::New();
	}

	bool EditorLayer::OpenProject()
	{
		std::string filepath = FileDialogs::OpenFile("Game Project (*.gameproj)\0*.gameproj\0");
		if (filepath.empty())
			return false;

		OpenProject(filepath);
		return true;
	}

	void EditorLayer::OpenProject(const std::filesystem::path& path)
	{
		if (Project::Load(path))
		{
			m_ContentBrowserPanel = CreateScope<ContentBrowserPanel>();
			ScriptEngine::Init();
			OpenScene(Project::GetActive()->GetConfig().StartScene);
		}
		else
		{
			// TODO prompt for new project
		}
	}

	void EditorLayer::SaveProject()
	{
		//Project::Save();
	}

	void EditorLayer::NewScene(const std::filesystem::path& path)
	{
		std::string filenameString = path.empty() ? "Untitled" : path.filename().string();

		m_EditorScene = CreateRef<Scene>(filenameString);
        m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x,(uint32_t)m_ViewportSize.y);
		m_EditorScenePath = path;
		m_ActiveScene = m_EditorScene;
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OpenScene()
	{
		std::string filepath = FileDialogs::OpenFile("Game Scene (*.scene)\0*.scene\0");
		if (!filepath.empty())
		{
			auto relativePath = std::filesystem::relative(filepath, Project::GetAssetDirectory());
			OpenScene(relativePath);
		}
	}

	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		if (path.extension().string() != ".scene")
		{
			ENGINE_CORE_WARN("Could not load {0} - not a scene file", path.filename().string());
			return;
		}

		if (m_SceneState != SceneState::Edit) OnSceneStop();

		NewScene(path);

		SceneSerializer serializer(m_ActiveScene);
		serializer.Deserialize(path);
	}

	void EditorLayer::SaveSceneAs()
	{
		std::filesystem::path filepath = FileDialogs::SaveFile("Game Scene (*.scene)\0*.scene\0");
		
		m_EditorScenePath = filepath;
		m_ActiveScene->SetSceneName(filepath.filename().string());

		SceneSerializer serializer(m_ActiveScene);
		serializer.Serialize(m_EditorScenePath.string());
	}

	void EditorLayer::SaveScene()
	{
		if (m_EditorScenePath.empty())
		{
			SaveSceneAs();
		}
		else
		{
			SceneSerializer serializer(m_ActiveScene);
			serializer.Serialize(m_EditorScenePath.string());
		}
	}

	void EditorLayer::OnDuplicateEntity()
	{
		if (m_SceneState != SceneState::Edit) return;

		if (m_SceneHierarchyPanel.IsSelectedEntityValid())
		{
			Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
			Entity newEntity = m_ActiveScene->DuplicateEntity(selectedEntity);
			m_SceneHierarchyPanel.SetSelectedEntity(newEntity);
		}
	}

	void EditorLayer::MousePicking()
	{
		// Mouse picking
		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		my = viewportSize.y - my;

		int mouseX = (int)mx;
		int mouseY = (int)my;

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
			int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
			m_HoveredEntityID = pixelData == -1 ? UUID::INVALID() : Entity((entt::entity)pixelData, m_ActiveScene.get()).GetUUID();
		}
		else
		{
			m_HoveredEntityID = UUID::INVALID();
		}
	}

	void EditorLayer::OnScenePlay()
	{
		if (m_SceneState == SceneState::Simulate)
			OnSceneStop();

		ENGINE_CORE_TRACE("SceneState changed to Play.");
		m_SceneState = SceneState::Play;

		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnRuntimeStart();

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnSceneSimulate()
	{
		if (m_SceneState == SceneState::Play)
			OnSceneStop();

		ENGINE_CORE_TRACE("SceneState changed to Simulate.");
		m_SceneState = SceneState::Simulate;

		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnSimulationStart();

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnSceneStop()
	{
		ENGINE_CORE_ASSERT(m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate);

		if (m_SceneState == SceneState::Play)
			m_ActiveScene->OnRuntimeStop();
		else if (m_SceneState == SceneState::Simulate)
			m_ActiveScene->OnSimulationStop();


		ENGINE_CORE_TRACE("SceneState changed to Edit.");
		m_SceneState = SceneState::Edit;
		m_ActiveScene->SetPaused(false);
		m_ActiveScene = m_EditorScene;

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

}

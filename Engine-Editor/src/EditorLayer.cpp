#include "EditorLayer.h"

#include "Engine/Renderer/Font.h"
#include "Engine/Asset/TextureImporter.h"

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

		m_IconPlay = TextureImporter::LoadTexture2D("Resources/Icons/PlayButton.png");
		m_IconPause = TextureImporter::LoadTexture2D("Resources/Icons/PauseButton.png");
		m_IconStep = TextureImporter::LoadTexture2D("Resources/Icons/StepButton.png");
		m_IconSimulate = TextureImporter::LoadTexture2D("Resources/Icons/SimulateButton.png");
		m_IconStop = TextureImporter::LoadTexture2D("Resources/Icons/StopButton.png");
		m_Outline = TextureImporter::LoadTexture2D("Resources/Icons/Outline.png");

		FramebufferSpecification fbSpec;
		fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Framebuffer = Framebuffer::Create(fbSpec);

		auto commandLineArgs = Application::Get().GetSpecification().CommandLineArgs;
		if (commandLineArgs.Count > 1)
		{
			auto projectFilePath = commandLineArgs[1];
			OpenProject(projectFilePath);

			if (m_EditorScene == nullptr)
				NewScene();
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
	}

	void EditorLayer::OnDetach()
	{
		ENGINE_CORE_WARN("Not Implemented!");
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		m_FrameTime = ts.GetMilliseconds();

		auto editorSceneManager = Project::GetActive()->GetEditorSceneManager();
		auto activeScene = editorSceneManager->GetActiveScene();

		activeScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		
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
		switch (editorSceneManager->GetEditorSceneState())
		{
			case EditorSceneState::Edit:
			{
				// Update
				m_EditorCamera.OnUpdate(ts);

				activeScene->OnUpdateEditor(ts, m_EditorCamera);

				if (m_EditorScene != activeScene)
					OpenScene(activeScene->Handle);

				break;
			}
			case EditorSceneState::Play:
			{
				activeScene->OnUpdateRuntime(ts);

				if (m_SceneHierarchyPanel.GetContext() != activeScene)
					m_SceneHierarchyPanel.SetContext(activeScene);

				break;
			}
			case EditorSceneState::Simulate:
			{
				m_EditorCamera.OnUpdate(ts);

				activeScene->OnUpdateSimulation(ts, m_EditorCamera);
				break;
			}
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

			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Project Settings"))
					m_ShowProjectSettingsWindow = true;

				ImGui::EndMenu();
			}
			
			if (ImGui::BeginMenu("Script"))
	        {

				if (ImGui::MenuItem("Reload Assembly", "Crtl+R"))
				{
					ScriptEngine::ReloadAssembly();
					SceneManager::LoadScene(m_EditorScene->Handle); // reload scene to match new assembly state
				}

	            ImGui::EndMenu();
	        }
			
			if (ImGui::BeginMenu("Window"))
	        {

				if (ImGui::MenuItem("Open Sprite View"))
					m_ShowSpriteWindow = true;

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
			Entity hoveredEntity = SceneManager::GetActiveScene()->GetEntityWithUUID(m_HoveredEntityID);
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
		ImGui::Checkbox("Toggle Gizmo Mode (World/Local)", &m_IsGizmoWorld);

		ImGui::Separator();
		ImGui::Text("UI Settings");
		ImGui::DragFloat2("Viewport Size", glm::value_ptr(m_ViewportSize));

		glm::vec2 windowSize{ Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight() };
		ImGui::DragFloat2("Viewport Size", glm::value_ptr(windowSize));

		ImGui::Separator();
		Ref<EditorAssetManager> editorAssetManager = Project::GetActive()->GetEditorAssetManager();

		{
			ImGui::BeginTable("Asset Registry", 3, ImGuiTableFlags_Resizable);

			ImGui::TableSetupColumn("Handle");
			ImGui::TableSetupColumn("Type");
			ImGui::TableSetupColumn("Path");
			ImGui::TableHeadersRow();

			int row = 0;
			for (const auto& [handle, metadata] : editorAssetManager->GetAssetRegistry())
			{
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text(std::to_string((uint64_t)handle).c_str());

				ImGui::TableSetColumnIndex(1);
				std::string assetTypeStr = Utils::AssetTypeToString(metadata.Type);
				ImGui::Text(assetTypeStr.c_str());

				ImGui::TableSetColumnIndex(2);
				ImGui::Text(metadata.Path.string().c_str());

				++row;
			}
			ImGui::EndTable();
		}

		{
			ImGui::BeginTable("Assets Loaded", 3, ImGuiTableFlags_Resizable);

			ImGui::TableSetupColumn("Handle");
			ImGui::TableSetupColumn("Type");
			ImGui::TableSetupColumn("Uses");
			ImGui::TableHeadersRow();

			int row = 0;
			for (const auto& [handle, asset] : editorAssetManager->GetLoadedAssets())
			{
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text(std::to_string((uint64_t)handle).c_str());

				ImGui::TableSetColumnIndex(1);
				std::string assetTypeStr = Utils::AssetTypeToString(asset->GetAssetType());
				ImGui::Text(assetTypeStr.c_str());

				ImGui::TableSetColumnIndex(2);
				ImGui::Text(std::to_string((long)asset.use_count()).c_str());

				++row;
			}
			ImGui::EndTable();
		}

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
				AssetHandle handle = *(AssetHandle*)payload->Data;
				Ref<EditorAssetManager> editorAssetManager = Project::GetActive()->GetEditorAssetManager();
				if (editorAssetManager->IsAssetHandleValid(handle))
				{
					auto type = editorAssetManager->GetAssetType(handle);
					if (type == AssetType::Scene)
						OpenScene(handle);
					else
						ENGINE_CORE_WARN("AssetType is not supported by drag and drop in the Viewport: {}", Utils::AssetTypeToString(type));
				}
				else
				{
					ENGINE_CORE_WARN("Asset was not valid. Check that it's been imported.");
				}
			}
			
			ImGui::EndDragDropTarget();
		}
		
		// Gizmos
		if(m_SceneHierarchyPanel.IsSelectedEntityValid() && m_GizmoType != -1)
		{
			Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
			bool isEntityUI = selectedEntity.HasComponent<UILayoutComponent>();

			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			
			ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

			// Editor Camera if world otherwise ScreenCamera
			const glm::mat4& cameraProjection = isEntityUI ? SceneManager::GetActiveScene()->GetScreenCamera().GetProjection() : m_EditorCamera.GetProjection();
			glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

			// Snapping
			bool snap = Input::IsKeyPressed(Key::LeftControl);
			float snapValue = 0.5f; // Snap to 0.5m for position/scale
			// Snap to 45 degrees for rotation
			if(m_GizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f;

			float snapValues[3] {snapValue, snapValue, snapValue};

			// Entity transform

			glm::mat4 transform = isEntityUI ? selectedEntity.GetUISpaceTransform() : selectedEntity.GetWorldSpaceTransform();
			glm::mat4 transformDelta{ 0.0f };

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), 
				(ImGuizmo::OPERATION)m_GizmoType, (ImGuizmo::MODE)m_IsGizmoWorld, glm::value_ptr(transform),
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

		if (m_ShowProjectSettingsWindow)
		{
			ImGui::Begin("Project Settings", &m_ShowProjectSettingsWindow);

			Ref<Project> project = Project::GetActive();

			if (project)
			{
				ProjectConfig& config = project->GetConfig();

				ImGui::Text("Project: ");
				ImGui::SameLine();
				ImGui::Text(config.Name.c_str());
				ImGui::Separator();

				Ref<EditorAssetManager> editorAssetManager = project->GetEditorAssetManager();

				{
					AssetHandle runtimeStartScene = config.RuntimeStartScene;
					ImGui::Text("Runtime Start Scene: ");
					ImGui::SameLine();
					std::filesystem::path scenePath = AssetManager::IsAssetHandleValid(runtimeStartScene) ? editorAssetManager->GetAssetPath(runtimeStartScene) : "None";
					ImGui::Text(scenePath.generic_string().c_str());
					ImGui::SameLine(); 
					ImGui::InputScalar("##RuntimeStartSceneHandle", ImGuiDataType_U64, &runtimeStartScene, nullptr, nullptr, nullptr, ImGuiInputTextFlags_ReadOnly);
					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
						{
							AssetHandle handle = *(AssetHandle*)payload->Data;
							if (editorAssetManager->IsAssetHandleValid(handle))
							{
								auto type = editorAssetManager->GetAssetType(handle);
								if (type == AssetType::Scene)
								{
									config.RuntimeStartScene = handle;
									project->Save();
								}
								else
								{
									ENGINE_CORE_WARN("AssetType is not a scene: {}", Utils::AssetTypeToString(type));
								}
							}
							else
							{
								ENGINE_CORE_WARN("Asset was not valid. Check that it's been imported.");
							}
						}

						ImGui::EndDragDropTarget();
					}
				}

				{
					AssetHandle editorStartScene = config.EditorStartScene;
					ImGui::Text("Editor Start Scene: ");
					ImGui::SameLine();
					std::filesystem::path scenePath = AssetManager::IsAssetHandleValid(editorStartScene) ? editorAssetManager->GetAssetPath(editorStartScene) : "None";
					ImGui::Text(scenePath.generic_string().c_str());
					ImGui::SameLine();
					ImGui::InputScalar("##StartSceneHandle", ImGuiDataType_U64, &editorStartScene, nullptr, nullptr, nullptr, ImGuiInputTextFlags_ReadOnly);
					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
						{
							AssetHandle handle = *(AssetHandle*)payload->Data;
							if (editorAssetManager->IsAssetHandleValid(handle))
							{
								auto type = editorAssetManager->GetAssetType(handle);
								if (type == AssetType::Scene)
								{
									config.EditorStartScene = handle;
									project->Save();
								}
								else
								{
									ENGINE_CORE_WARN("AssetType is not a scene: {}", Utils::AssetTypeToString(type));
								}
							}
							else
							{
								ENGINE_CORE_WARN("Asset was not valid. Check that it's been imported.");
							}
						}

						ImGui::EndDragDropTarget();
					}
				}

				ImGui::Text("Project Directory: ");
				ImGui::SameLine();
				ImGui::Text(Project::GetProjectDirectory().generic_string().c_str());

				ImGui::Text("Asset Directory: ");
				ImGui::SameLine();
				ImGui::Text(Project::GetAssetDirectory().generic_string().c_str());

				ImGui::Text("Asset Registry Path: ");
				ImGui::SameLine();
				ImGui::Text(Project::GetAssetRegistryPath().generic_string().c_str());

				ImGui::Text("Script Module Path: ");
				ImGui::SameLine();
				ImGui::Text(Project::GetAssetFileSystemPath(config.ScriptModulePath).generic_string().c_str());
			}

			ImGui::End();
		}

		if (m_ShowSpriteWindow)
		{
			ImGui::Begin("Sprite View", &m_ShowSpriteWindow);

			if (m_SceneHierarchyPanel.IsSelectedEntityValid())
			{
				Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
				if (selectedEntity.HasComponent<SpriteRendererComponent>())
				{
					SpriteRendererComponent sprite = selectedEntity.GetComponent<SpriteRendererComponent>();

					if (sprite.Texture.IsValid())
					{
						Ref<Texture2D> spriteTexture = AssetManager::GetAsset<Texture2D>(sprite.Texture);

						float spriteWidth = spriteTexture->GetWidth();
						float spriteHeight = spriteTexture->GetHeight();
						glm::vec2 imageRegion = { spriteWidth, spriteHeight};

						ImVec2 regionAvailable = ImGui::GetContentRegionAvail();
						float regionSmallest = glm::min(regionAvailable.x, regionAvailable.y);
						float scale = (spriteWidth / regionSmallest);
						imageRegion /= scale;

						ImVec2 screenPos = ImGui::GetCursorScreenPos();

						ImGui::Image((ImTextureID)spriteTexture->GetRendererID(), { imageRegion.x, imageRegion.y }, { 0, 1 }, { 1, 0 });
						
						if (sprite.IsSubTexture && sprite.SubTexture)
						{
							glm::vec2 subTextMin{ (sprite.SubCoords.x * sprite.SubCellSize.x), (sprite.SubCoords.y * sprite.SubCellSize.y) };
							glm::vec2 subTextMax{ ((sprite.SubCoords.x + sprite.SubSpriteSize.x) * sprite.SubCellSize.x), ((sprite.SubCoords.y + sprite.SubSpriteSize.y) * sprite.SubCellSize.y) };
							subTextMin /= scale;
							subTextMax /= scale;

							glm::vec2 min{ screenPos.x + subTextMin.x, (screenPos.y + imageRegion.y) - subTextMin.y };
							glm::vec2 max{ screenPos.x + subTextMax.x, (screenPos.y + imageRegion.y) - subTextMax.y };

							ImGui::GetWindowDrawList()->AddImage((ImTextureID)m_Outline->GetRendererID(), { min.x, min.y }, { max.x, max.y }, { 0, 1 }, { 1, 0 });
						}
					}
				}
			}

			ImGui::End();
		}
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

		auto editorSceneManager = Project::GetActive()->GetEditorSceneManager();
		auto sceneState = editorSceneManager->GetEditorSceneState();
		auto activeScene = editorSceneManager->GetActiveScene();

		// play button
		if (sceneState != EditorSceneState::Simulate)
		{
			Ref<Texture2D> icon = sceneState == EditorSceneState::Edit ? m_IconPlay : m_IconStop;
			if (ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
			{
				if (sceneState == EditorSceneState::Play)
					OnSceneStop();
				else
					OnScenePlay();
			}
		}

		// simulate button
		if (sceneState != EditorSceneState::Play)
		{
			if (sceneState == EditorSceneState::Edit)
				ImGui::SameLine();

			Ref<Texture2D> icon = sceneState == EditorSceneState::Edit ? m_IconSimulate : m_IconStop;
			if (ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
			{
				if (sceneState == EditorSceneState::Simulate)
					OnSceneStop();
				else
					OnSceneSimulate();
			}
		}

		if (sceneState != EditorSceneState::Edit)
		{
			// pause button
			ImGui::SameLine();
			{
				Ref<Texture2D> icon = m_IconPause;
				bool isPaused = activeScene->IsPaused();
				ImVec4 tintColor = isPaused ? ImVec4(0.25f, 0.25f, 1, 1) : ImVec4(1, 1, 1, 1);
				if (ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0), tintColor))
					activeScene->SetPaused(!isPaused);
			}
			
			// step button
			if (activeScene->IsPaused())
			{
				ImGui::SameLine();
				{
					Ref<Texture2D> icon = m_IconStep;
					if (ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
					{
						activeScene->Step();
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
		dispatcher.Dispatch<WindowDropEvent>(ENGINE_BIND_EVENT_FN(EditorLayer::OnWindowDrop));
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
					SceneManager::LoadScene(m_EditorScene->Handle); // reload scene to match new assembly state
				}
				else
				{
					if (!m_IsGizmoInUse)
						m_GizmoType = ImGuizmo::OPERATION::SCALE;
				}
				break;
			}
			case Key::T:
			{
				m_IsGizmoWorld = !m_IsGizmoWorld;
			}

			// Other
			case Key::D:
			{
				if (control)
					OnDuplicateEntity();

				break;
			}
			case Key::F:
			{
				if (m_SceneHierarchyPanel.IsSelectedEntityValid())
				{
					Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
					m_EditorCamera.SetFocusTarget(Math::PositionFromTransform(selectedEntity.GetWorldSpaceTransform()), 10.0f);
				}
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
		if (e.GetMouseButton() == Mouse::ButtonLeft)
		{
			if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt))
				m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntityID);
		}
		
		return false;
	}

	bool EditorLayer::OnWindowDrop(WindowDropEvent& e)
	{
		Ref<EditorAssetManager> editorManager = Project::GetActive()->GetEditorAssetManager();

		for (auto& fullPath : e.GetPaths())
		{
			ENGINE_CORE_TRACE("Dropped: " + fullPath.string());
			editorManager->ImportAsset(fullPath);
		}

		return true;
	}

	void EditorLayer::OnOverlayRender()
	{
		auto editorSceneManager = Project::GetActive()->GetEditorSceneManager();
		auto sceneState = editorSceneManager->GetEditorSceneState();
		auto activeScene = editorSceneManager->GetActiveScene();

		if (sceneState == EditorSceneState::Play)
		{
			Entity camera = activeScene->GetPrimaryCameraEntity();
			if (!camera) return;

			Renderer2D::BeginScene(camera.GetComponent<CameraComponent>().Camera, camera.GetWorldSpaceTransform());
		}
		else
		{
			Renderer2D::BeginScene(m_EditorCamera);
		}

		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		bool isEntityUI = m_SceneHierarchyPanel.IsSelectedEntityValid() && selectedEntity.HasComponent<UILayoutComponent>();

		if (m_ShowPhysicsColliders)
		{
			{ // Visualize Box Collider 2D
				auto view = activeScene->GetAllEntitiesWith<TransformComponent, BoxCollider2DComponent>();

				for (auto entity : view)
				{
					auto [tc, bc2d] = view.get<TransformComponent, BoxCollider2DComponent>(entity);

					glm::vec3 position = tc.Position + glm::vec3(glm::rotate(bc2d.Offset, tc.Rotation.z), 0.001f);
					glm::vec3 scale = tc.Scale * glm::vec3(bc2d.Size * 2.0f, 1.0f);

					glm::mat4 transform = Math::GenRectTransform(position, tc.Rotation.z, scale);

					Renderer2D::DrawRect(transform, glm::vec4(0, 1, 0, 1));
				}
			}

			{ // Visualize Circle Collider 2D
				auto view = activeScene->GetAllEntitiesWith<TransformComponent, CircleCollider2DComponent>();

				for (auto entity : view)
				{
					auto [tc, cc2d] = view.get<TransformComponent, CircleCollider2DComponent>(entity);

					glm::vec3 position = tc.Position + glm::vec3(glm::rotate(cc2d.Offset, tc.Rotation.z), 0.001f);
					glm::vec3 scale = tc.Scale * glm::vec3(cc2d.Radius * 2.0f);

					glm::mat4 transform = Math::GenRectTransform(position, 0, scale);

					Renderer2D::DrawCircle(transform, glm::vec4(0, 1, 0, 1), 0.05f);
				}
			}
		}

		// Draw selected entity outline
		if (m_SceneHierarchyPanel.IsSelectedEntityValid() && !isEntityUI)
		{
			Renderer2D::SetLineWidth(4.0f);
			Renderer2D::DrawRect(selectedEntity.GetWorldSpaceTransform(), glm::vec4(1, 0, 0.5f, 1));
		}
		else
		{
			Renderer2D::SetLineWidth(2.0f);
		}

		Renderer2D::EndScene();

		// UI Overlays

		Renderer2D::BeginScene(activeScene->GetScreenCamera(), glm::mat4(1.0f));

		// Draw selected entity outline
		if (m_SceneHierarchyPanel.IsSelectedEntityValid() && isEntityUI)
		{
			Renderer2D::SetLineWidth(4.0f);
			Renderer2D::DrawRect(selectedEntity.GetUISpaceTransform(), glm::vec4(1, 0, 0.5f, 1));
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

			AssetHandle startScene = Project::GetActive()->GetConfig().EditorStartScene;
			AssetManager::IsAssetHandleValid(startScene) ? OpenScene(startScene) : NewScene();
		}
		else
		{
			// TODO prompt for new project
		}
	}

	void EditorLayer::SaveProject()
	{
		Project::Save();
	}

	void EditorLayer::NewScene()
	{
		if (Project::GetActive()->GetEditorSceneManager()->GetEditorSceneState() != EditorSceneState::Edit) OnSceneStop();

		m_EditorScene = SceneManager::CreateNewScene();
        m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x,(uint32_t)m_ViewportSize.y);
        m_SceneHierarchyPanel.SetContext(m_EditorScene);
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

		AssetHandle sceneHandle = Project::GetActive()->GetEditorAssetManager()->GetAssetHandleFromFilePath(path);
		if (sceneHandle.IsValid())
			OpenScene(sceneHandle);
	}

	void EditorLayer::OpenScene(const AssetHandle handle)
	{
		if (Project::GetActive()->GetEditorSceneManager()->GetEditorSceneState() != EditorSceneState::Edit) OnSceneStop();

		m_EditorScene = SceneManager::LoadScene(handle);
		m_SceneHierarchyPanel.SetContext(m_EditorScene);
	}

	void EditorLayer::SaveSceneAs()
	{
		if (Project::GetActive()->GetEditorSceneManager()->GetEditorSceneState() != EditorSceneState::Edit) return;

		std::filesystem::path fullPath = FileDialogs::SaveFile("Game Scene (*.scene)\0*.scene\0");

		if (fullPath.empty())
			return;

		if (fullPath.extension() != ".scene")
			fullPath += ".scene";

		std::filesystem::path relativePath = std::filesystem::relative(fullPath, Project::GetAssetDirectory());

		SceneManager::GetActiveScene()->SetSceneName(relativePath.filename().generic_string());

		AssetMetadata metadata = AssetMetadata();
		metadata.Path = fullPath;
		metadata.Type = AssetType::Scene;

		Project::GetActive()->GetEditorAssetManager()->SaveAssetAs(m_EditorScene, relativePath.generic_string());
		m_ContentBrowserPanel->RefreshAssetTree();
	}

	void EditorLayer::SaveScene()
	{
		if (Project::GetActive()->GetEditorSceneManager()->GetEditorSceneState() != EditorSceneState::Edit) return;

		Ref<EditorAssetManager> editorAssetManager = Project::GetActive()->GetEditorAssetManager();
		if (editorAssetManager->IsAssetHandleValid(m_EditorScene->Handle))
			editorAssetManager->SaveAsset(m_EditorScene);
		else
			SaveSceneAs();
	}

	void EditorLayer::OnDuplicateEntity()
	{
		if (Project::GetActive()->GetEditorSceneManager()->GetEditorSceneState() != EditorSceneState::Edit) return;

		if (m_SceneHierarchyPanel.IsSelectedEntityValid())
		{
			Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
			Entity newEntity = SceneManager::GetActiveScene()->DuplicateEntity(selectedEntity);
			m_SceneHierarchyPanel.SetSelectedEntity(newEntity);
		}
	}

	void EditorLayer::MousePicking()
	{
		auto scene = SceneManager::GetActiveScene();

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

			if (pixelData > -1 && scene->IsEntityHandleValid((entt::entity)pixelData))
				m_HoveredEntityID = Entity((entt::entity)pixelData, scene.get()).GetUUID();
			else
				m_HoveredEntityID = UUID::INVALID();
		}
		else
		{
			m_HoveredEntityID = UUID::INVALID();
		}

		if (scene->IsRunning())
			UIEngine::SetViewportMousePos(mouseX, mouseY);
	}

	void EditorLayer::OnScenePlay()
	{
		auto editorSceneManager = Project::GetActive()->GetEditorSceneManager();
		auto sceneState = editorSceneManager->GetEditorSceneState();
		if (sceneState == EditorSceneState::Simulate)
			OnSceneStop();

		SaveScene();

		ENGINE_CORE_TRACE("SceneState changed to Play.");
		editorSceneManager->SetEditorSceneState(EditorSceneState::Play);

		auto& scene = editorSceneManager->LoadSceneCopy(m_EditorScene);
		scene->OnRuntimeStart();

		m_SceneHierarchyPanel.SetContext(scene);
	}

	void EditorLayer::OnSceneSimulate()
	{
		auto editorSceneManager = Project::GetActive()->GetEditorSceneManager();
		auto sceneState = editorSceneManager->GetEditorSceneState();
		if (sceneState == EditorSceneState::Play)
			OnSceneStop();

		ENGINE_CORE_TRACE("SceneState changed to Simulate.");
		editorSceneManager->SetEditorSceneState(EditorSceneState::Simulate);

		auto& scene = editorSceneManager->LoadSceneCopy(m_EditorScene);
		scene->OnSimulationStart();

		m_SceneHierarchyPanel.SetContext(scene);
	}

	void EditorLayer::OnSceneStop()
	{
		auto editorSceneManager = Project::GetActive()->GetEditorSceneManager();
		auto sceneState = editorSceneManager->GetEditorSceneState();
		ENGINE_CORE_ASSERT(sceneState == EditorSceneState::Play || sceneState == EditorSceneState::Simulate);

		auto& scene = editorSceneManager->GetActiveScene();
		if (sceneState == EditorSceneState::Play)
			scene->OnRuntimeStop();
		else if (sceneState == EditorSceneState::Simulate)
			scene->OnSimulationStop();


		ENGINE_CORE_TRACE("SceneState changed to Edit.");
		editorSceneManager->SetEditorSceneState(EditorSceneState::Edit);
		scene->SetPaused(false);
		m_EditorScene = editorSceneManager->LoadScene(m_EditorScene->Handle);

		m_SceneHierarchyPanel.SetContext(m_EditorScene);
	}

}

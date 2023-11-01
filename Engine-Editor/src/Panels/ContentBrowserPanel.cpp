#include "ContentBrowserPanel.h"
#include "Engine/Asset/TextureImporter.h"

#include <imgui/imgui.h>

namespace Engine
{
	ContentBrowserPanel::ContentBrowserPanel(Ref<Project> project)
		:m_Project(project), m_ThumbnailCache(CreateRef<ThumbnailCache>(project)), m_BaseDirectory(m_Project->GetAssetDirectory()), m_CurrentDirectory(m_BaseDirectory)
	{
		m_TreeNodes.emplace_back(".", AssetHandle::INVALID());

		m_DirectoryIcon = TextureImporter::LoadTexture2D("Resources/Icons/ContentBrowser/DirectoryIcon.png");
		m_FileIcon = TextureImporter::LoadTexture2D("Resources/Icons/ContentBrowser/FileIcon.png");

		RefreshAssetTree();
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		if (m_BaseDirectory.empty() || m_CurrentDirectory.empty()) return;

		ImGui::Begin("Content Browser");

		const char* label = m_Mode == Mode::Asset ? "Asset" : "File";
		if (ImGui::Button(label))
			m_Mode = m_Mode == Mode::Asset ? Mode::FileSystem : Mode::Asset;

		if (m_CurrentDirectory != m_BaseDirectory)
		{
			ImGui::SameLine();
			if (ImGui::Button("<-"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}

		static float padding = 16.0f;
		static float thumbnailSize = 64;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columCount = (int)(panelWidth / cellSize);
		if (columCount < 1) columCount = 1;
		ImGui::BeginTable("Content Browser", columCount);

		if (m_Mode == Mode::Asset)
		{
			TreeNode* node = &m_TreeNodes[0];

			auto currentDir = std::filesystem::relative(m_CurrentDirectory, m_Project->GetAssetDirectory());
			for (const auto& path : currentDir)
			{
				// if only one level
				if (node->Path == currentDir)
					break;

				if (node->Children.find(path) != node->Children.end())
				{
					node = &m_TreeNodes[node->Children[path]];
					continue;
				}
				else
				{
					// can't find path
					m_CurrentDirectory = m_BaseDirectory; // reset
				}
			}

			for (const auto& [item, treeNodeIndex] : node->Children)
			{
				std::string itemStr = item.generic_string();
				bool isDirectory = std::filesystem::is_directory(m_Project->GetAssetDirectory() / item);
				AssetHandle indexHandle = m_TreeNodes[treeNodeIndex].Handle;

				ImGui::PushID(itemStr.c_str());
				ImGui::TableNextColumn();


				Ref<Texture2D> icon;
				if (isDirectory)
				{
					icon = m_DirectoryIcon;
				}
				else
				{
					icon = m_ThumbnailCache->GetThumbnail(indexHandle);
					if (icon == nullptr)
					{
						icon = m_FileIcon;
					}
				}
				
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
				ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

				if (ImGui::BeginDragDropSource())
				{
					ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", &indexHandle, sizeof(AssetHandle), ImGuiCond_Once);
					ImGui::EndDragDropSource();
				}

				ImGui::PopStyleColor();
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					if (isDirectory)
					{
						m_CurrentDirectory /= item.filename();
					}
					else if (Project::GetActive()->GetEditorAssetManager()->GetAssetType(indexHandle) == AssetType::Scene)
					{
						if (Project::GetActive()->GetEditorSceneManager()->GetEditorSceneState() == EditorSceneState::Edit)
							SceneManager::LoadScene(indexHandle);
					}
				}

				ImGui::TextWrapped(itemStr.c_str());

				ImGui::PopID();
			}
		}
		else
		{
			Ref<EditorAssetManager> editorAssetManager = Project::GetActive()->GetEditorAssetManager();

			uint32_t count = 0;
			for (const auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
			{
				count++;
			}

			ImGuiListClipper clipper(glm::ceil((float)count / (float)columCount));
			bool first = true;
			while (clipper.Step())
			{
				auto it = std::filesystem::directory_iterator(m_CurrentDirectory);
				if (!first)
				{
					for (int i  = 0; i < clipper.DisplayStart; i++)
					{
						for (int c = 0; c < columCount && it != std::filesystem::directory_iterator(); c++)
						{
							it++;
						}
					}
				}

				for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
				{
					int c;
					for (c = 0; c < columCount && it != std::filesystem::directory_iterator(); c++, it++)
					{
						const auto& directoryEntry = *it;

						const auto& path = directoryEntry.path();
						std::string filenameString = path.filename().string();
						bool isDirectory = directoryEntry.is_directory();
						const auto& relativePath = std::filesystem::relative(path, Project::GetActiveAssetDirectory());

						ImGui::PushID(filenameString.c_str());
						ImGui::TableNextColumn();

						Ref<Texture2D> icon;
						if (isDirectory)
						{
							icon = m_DirectoryIcon;
						}
						else
						{
							icon = m_ThumbnailCache->GetThumbnail(relativePath);
							if (icon == nullptr)
							{
								icon = m_FileIcon;
							}
						}

						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
						ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

						const auto& assetType = editorAssetManager->GetAssetTypeFromFileExtension(relativePath.extension());
						if (assetType != AssetType::None && ImGui::BeginPopupContextItem())
						{
							if (ImGui::MenuItem("Import Asset"))
							{
								editorAssetManager->ImportAsset(relativePath);
								RefreshAssetTree();
							}

							ImGui::EndPopup();
						}

						ImGui::PopStyleColor();
						if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
						{
							if (isDirectory)
								m_CurrentDirectory /= relativePath.filename();
						}

						ImGui::TextWrapped(filenameString.c_str());

						ImGui::PopID();

					}

					if (first && c < columCount)
					{
						for (int extra = 0; extra < columCount - c; extra++)
						{
							ImGui::TableNextColumn();
						}
					}
				}
			}
		}
		
		ImGui::EndTable();
		
		ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 32, 164);
		// ImGui::SliderFloat("Padding Size", &padding, 0, 32);
		// TODO: Status bar
		
		ImGui::End();

		m_ThumbnailCache->OnUpdate();
	}

	void ContentBrowserPanel::RefreshAssetTree()
	{
		const auto& assetRegistry = Project::GetActive()->GetEditorAssetManager()->GetAssetRegistry();
		for (const auto&[handle, metadata] : assetRegistry)
		{
			uint32_t currentNodeIndex = 0;

			for (const auto& path : metadata.Path)
			{
				auto it = m_TreeNodes[currentNodeIndex].Children.find(path.generic_string());
				if (it != m_TreeNodes[currentNodeIndex].Children.end())
				{
					currentNodeIndex = it->second;
				}
				else
				{
					// add node
					TreeNode newNode(path, handle);
					newNode.Parent = currentNodeIndex;
					m_TreeNodes.push_back(newNode);

					m_TreeNodes[currentNodeIndex].Children[path] = m_TreeNodes.size() - 1;
					currentNodeIndex = m_TreeNodes.size() - 1;
				}
			}
		}
	}
}

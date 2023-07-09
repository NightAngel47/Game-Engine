﻿#include "ContentBrowserPanel.h"
#include "Engine/Asset/TextureImporter.h"

#include <imgui/imgui.h>

namespace Engine
{
	ContentBrowserPanel::ContentBrowserPanel()
		:m_BaseDirectory(Project::GetAssetDirectory()), m_CurrentDirectory(m_BaseDirectory)
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

			auto currentDir = std::filesystem::relative(m_CurrentDirectory, Project::GetAssetDirectory());
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
				bool isDirectory = std::filesystem::is_directory(Project::GetAssetDirectory() / item);

				ImGui::PushID(itemStr.c_str());

				ImGui::TableNextColumn();

				Ref<Texture2D> icon = isDirectory ? m_DirectoryIcon : m_FileIcon;
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
				ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

				AssetHandle indexHandle = m_TreeNodes[treeNodeIndex].Handle;

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
					// TODO fix scene hierarchy panel not updating as well when this is done
					//else if (Project::GetActive()->GetEditorAssetManager()->GetAssetType(indexHandle) == AssetType::Scene)
					//{
					//	SceneManager::LoadScene(indexHandle);
					//}
				}

				ImGui::TextWrapped(itemStr.c_str());

				ImGui::PopID();
			}
		}
		else
		{
			Ref<EditorAssetManager> editorAssetManager = Project::GetActive()->GetEditorAssetManager();

			for (const auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
			{
				const auto& path = directoryEntry.path();
				std::string filenameString = path.filename().string();
				bool isDirectory = directoryEntry.is_directory();

				ImGui::PushID(filenameString.c_str());

				ImGui::TableNextColumn();

				Ref<Texture2D> icon = isDirectory ? m_DirectoryIcon : m_FileIcon;
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
				ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

				const auto& assetType = editorAssetManager->GetAssetTypeFromFileExtension(path.extension());
				if (assetType != AssetType::None && ImGui::BeginPopupContextItem())
				{
					if (ImGui::MenuItem("Import Asset"))
					{
						editorAssetManager->ImportAsset(path);
						RefreshAssetTree();
					}

					ImGui::EndPopup();
				}

				ImGui::PopStyleColor();
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					if (isDirectory)
						m_CurrentDirectory /= path.filename();
				}

				ImGui::TextWrapped(filenameString.c_str());

				ImGui::PopID();
			}
		}
		
		ImGui::EndTable();
		
		ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 32, 164);
		// ImGui::SliderFloat("Padding Size", &padding, 0, 32);
		// TODO: Status bar
		
		ImGui::End();
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

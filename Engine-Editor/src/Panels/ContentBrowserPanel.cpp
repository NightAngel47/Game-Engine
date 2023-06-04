#include "ContentBrowserPanel.h"
#include "Engine/Asset/TextureImporter.h"

#include <imgui/imgui.h>

namespace Engine
{
	ContentBrowserPanel::ContentBrowserPanel()
		:m_BaseDirectory(Project::GetAssetDirectory()), m_CurrentDirectory(m_BaseDirectory)
	{
		//m_DirectoryIcon = Texture2D::Create("Resources/Icons/ContentBrowser/DirectoryIcon.png");
		//m_FileIcon = Texture2D::Create("Resources/Icons/ContentBrowser/FileIcon.png");

		{
			AssetMetadata metadata = AssetMetadata();
			metadata.Path = "Resources/Icons/ContentBrowser/DirectoryIcon.png";
			metadata.Type = AssetType::Texture2D;
			m_DirectoryIcon = TextureImporter::ImportTexture2D(AssetHandle(), metadata, true);
		}
		{
			AssetMetadata metadata = AssetMetadata();
			metadata.Path = "Resources/Icons/ContentBrowser/FileIcon.png";
			metadata.Type = AssetType::Texture2D;
			m_FileIcon = TextureImporter::ImportTexture2D(AssetHandle(), metadata, true);
		}
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		if (m_BaseDirectory.empty() || m_CurrentDirectory.empty()) return;

		ImGui::Begin("Content Browser");

		if (m_CurrentDirectory != m_BaseDirectory)
		{
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

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = directoryEntry.path();
			std::string filenameString = path.filename().string();

			ImGui::PushID(filenameString.c_str());
			
			ImGui::TableNextColumn();

			Ref<Texture2D> icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

			if (ImGui::BeginDragDropSource())
			{
				auto relativePath = std::filesystem::relative(path, m_BaseDirectory);
				const wchar_t* itemPath = relativePath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t), ImGuiCond_Once);
				ImGui::EndDragDropSource();
			}
			
			ImGui::PopStyleColor();
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (directoryEntry.is_directory())
				{
					m_CurrentDirectory /= filenameString;
				}
			}
			
			ImGui::TextWrapped(filenameString.c_str());

			ImGui::PopID();
		}
		
		ImGui::EndTable();
		
		ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 32, 164);
		//ImGui::SliderFloat("Padding Size", &padding, 0, 32);
		// TODO: Status bar
		
		ImGui::End();
	}
}

#include "enginepch.h"
#include "Engine/Renderer/SubTexture2D.h"
#include "Engine/Asset/AssetManager.h"

namespace Engine
{
	SubTexture2D::SubTexture2D(const Ref<Texture2D> texture, const glm::vec2& min, const glm::vec2& max)
		:m_Texture(texture)
	{
		m_TexCoords[0] = {min.x, min.y};
		m_TexCoords[1] = {max.x, min.y};
		m_TexCoords[2] = {max.x, max.y};
		m_TexCoords[3] = {min.x, max.y};
	}

	Ref<SubTexture2D> SubTexture2D::CreateFromCoords(const AssetHandle texture, const glm::vec2& coords, const glm::vec2& cellSize, const glm::vec2& spriteSize)
	{
		Ref<Texture2D> textureAsset = AssetManager::GetAsset<Texture2D>(texture);

		uint32_t width = textureAsset->GetWidth();
		uint32_t height = textureAsset->GetHeight();

		glm::vec2 min = { (coords.x * cellSize.x) / width, (coords.y * cellSize.y) / height };
		glm::vec2 max = { ((coords.x + spriteSize.x) * cellSize.x) / width, ((coords.y + spriteSize.y) * cellSize.y) / height };
		return CreateRef<SubTexture2D>(textureAsset, min, max);
	}
	
}

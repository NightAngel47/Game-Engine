#pragma once
#include "Engine/Renderer/Texture.h"

#include <glm/glm.hpp>

namespace Engine
{
	class SubTexture2D
	{
	public:
		SubTexture2D(const AssetHandle texture, const glm::vec2& min, const glm::vec2& max);

		//const Ref<Texture2D> GetTexture() const { return AssetManager::GetAsset<Texture2D>(m_Texture); }
		const AssetHandle GetTexture() const { return m_Texture; }
		const glm::vec2* GetTexCoords() const { return m_TexCoords; }

		static Ref<SubTexture2D> CreateFromCoords(const AssetHandle texture, const glm::vec2& coords, const glm::vec2& cellSize, const glm::vec2& spriteSize = {1, 1});
	private:
		AssetHandle m_Texture;

		glm::vec2 m_TexCoords[4];
	};
	
}

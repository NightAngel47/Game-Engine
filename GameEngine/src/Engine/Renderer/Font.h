#pragma once
#include "Engine/Renderer/Texture.h"

namespace Engine
{
	struct MSDFData;

	class Font
	{
	public:
		Font(const std::filesystem::path& filepath);
		~Font();

		Ref<Texture2D> GetAtlasTexture() const { return m_AtlasTexture; }
	private:
		MSDFData* m_Data = nullptr;
		Ref<Texture2D> m_AtlasTexture;
	};
}

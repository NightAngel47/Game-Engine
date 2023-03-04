#pragma once

#include "Engine/Renderer/Texture.h"

#include <glad/glad.h>

namespace Engine
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const TextureSpecification& specification);
		OpenGLTexture2D(const std::filesystem::path& path);
		virtual ~OpenGLTexture2D();

		virtual const TextureSpecification& GetSpecification() const { return m_Specification; }

		virtual uint32_t GetWidth() const override { return m_Specification.Width; }
		virtual uint32_t GetHeight() const override { return m_Specification.Height; }
		virtual uint32_t GetRendererID() const override { return m_RendererID; }
		
		virtual void SetData(void* data, uint32_t size) override;
		
		virtual void Bind(uint32_t slot = 0) const override;
		
		virtual bool operator==(const Texture& other) const override
		{
			return m_RendererID == other.GetRendererID();
		}

	private:
		std::filesystem::path m_Path;
		TextureSpecification m_Specification;
		uint32_t m_Width;
		uint32_t m_Height;
		uint32_t m_RendererID{};
		GLenum m_InternalFormat, m_DataFormat;
	};
}

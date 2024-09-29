#pragma once
#include "Engine/Renderer/Texture.h"

namespace Engine
{
	class VulkanTexture2D : public Texture2D
	{
	public:
		VulkanTexture2D() = default;
		VulkanTexture2D(const TextureSpecification& specification, Buffer data = Buffer());
		virtual ~VulkanTexture2D();

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual uint32_t GetRendererID() const override { return m_RendererID; }
		
		void ChangeSize(uint32_t newWidth, uint32_t newHeight) override;

		virtual void SetData(Buffer data) override;
		
		virtual void Bind(uint32_t slot = 0) const override;
		
		virtual bool operator==(const Texture& other) const override
		{
			return m_RendererID == other.GetRendererID();
		}

	private:
		uint32_t m_Width;
		uint32_t m_Height;
		uint32_t m_RendererID{};
		//GLenum m_InternalFormat, m_DataFormat;
	};
}

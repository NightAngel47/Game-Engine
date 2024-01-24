#include "enginepch.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Engine
{
	namespace Utils
	{
		static GLenum EngineImageFormatToGLDataFormat(ImageFormat format)
		{
			switch (format)
			{
			case Engine::ImageFormat::R8:
				return GL_R;
			case Engine::ImageFormat::RGB8:
				return GL_RGB;
			case Engine::ImageFormat::RGBA8:
			case Engine::ImageFormat::RGBA32F:
				return GL_RGBA;
			case Engine::ImageFormat::None:
			default:
				ENGINE_CORE_ASSERT(false, "Image Format not specified.");
				break;
			}

			return GL_RGBA; // default
		}

		static GLenum EngineImageFormatToGLInternalFormat(ImageFormat format)
		{
			switch (format)
			{
			case Engine::ImageFormat::R8:
				return GL_R8;
			case Engine::ImageFormat::RGB8:
				return GL_RGB8;
			case Engine::ImageFormat::RGBA8:
				return GL_RGBA8;
			case Engine::ImageFormat::RGBA32F:
				return GL_RGBA32F;
			case Engine::ImageFormat::None:
			default:
				ENGINE_CORE_ASSERT(false, "Image Format not specified.");
				break;
			}

			return GL_RGBA8; // default
		}

		static ImageFormat GLDataFormatToEngineImageFormat(GLenum format)
		{
			switch (format)
			{
			case GL_R:
				return Engine::ImageFormat::R8;
			case GL_RGB:
				return Engine::ImageFormat::RGB8;
			case GL_RGBA:
				return Engine::ImageFormat::RGBA8; // Engine::ImageFormat::RGBA32F;
			default:
				ENGINE_CORE_ASSERT(false, "Image Format not specified.");
				break;
			}

			return ImageFormat::RGBA8; // default
		}

		static ImageFormat GLInternalFormatToEngineImageFormat(GLenum format)
		{
			switch (format)
			{
			case GL_R8:
				return Engine::ImageFormat::R8;
			case GL_RGB8:
				return Engine::ImageFormat::RGB8;
			case GL_RGBA8:
				return Engine::ImageFormat::RGBA8;
			case GL_RGBA32F:
				return Engine::ImageFormat::RGBA32F;
			default:
				ENGINE_CORE_ASSERT(false, "Image Format not specified.");
				break;
			}

			return ImageFormat::RGBA8; // default
		}
	}

	OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& specification, Buffer data)
		: m_Width(specification.Width), m_Height(specification.Height)
	{
		ENGINE_PROFILE_FUNCTION();

		m_InternalFormat = Utils::EngineImageFormatToGLInternalFormat(specification.Format);
		m_DataFormat = Utils::EngineImageFormatToGLDataFormat(specification.Format);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		if (data)
			SetData(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		ENGINE_PROFILE_FUNCTION();
		
		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::ChangeSize(uint32_t newWidth, uint32_t newHeight)
	{
		// Create new texture
		uint32_t newTextureID;
		glCreateTextures(GL_TEXTURE_2D, 1, &newTextureID);
		glTextureStorage2D(newTextureID, 1, m_InternalFormat, newWidth, newHeight);

		glTextureParameteri(newTextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(newTextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(newTextureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(newTextureID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// downsample
		GLuint framebufferRendererIDs[2] = { 0 };
		glGenFramebuffers(2, framebufferRendererIDs);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferRendererIDs[0]);
		glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_RendererID, 0);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebufferRendererIDs[1]);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, newTextureID, 0);

		glBlitFramebuffer(0, 0, m_Width, m_Height, 0, 0, newWidth, newHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		// swap textures
		glDeleteTextures(1, &m_RendererID);
		glDeleteFramebuffers(2, framebufferRendererIDs);
		m_RendererID = newTextureID;
		m_Width = newWidth;
		m_Height = newHeight;
	}

	void OpenGLTexture2D::SetData(Buffer data) 
	{
		ENGINE_PROFILE_FUNCTION();
		
		uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
		ENGINE_CORE_ASSERT(data.Size == m_Width * m_Height * bpp, "Data must be entire texture!");
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data.Data);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		ENGINE_PROFILE_FUNCTION();
		
		glBindTextureUnit(slot, m_RendererID);
	}

}

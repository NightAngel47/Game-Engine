#include "enginepch.h"
#include "Platform/Vulkan/VulkanTexture.h"

namespace Engine
{
	VulkanTexture2D::VulkanTexture2D(const TextureSpecification& specification, Buffer data)
		: m_Width(specification.Width), m_Height(specification.Height)
	{
		ENGINE_PROFILE_FUNCTION();
	}

	VulkanTexture2D::~VulkanTexture2D()
	{
		ENGINE_PROFILE_FUNCTION();
	}

	void VulkanTexture2D::ChangeSize(uint32_t newWidth, uint32_t newHeight)
	{
		ENGINE_PROFILE_FUNCTION();
	}

	void VulkanTexture2D::SetData(Buffer data)
	{
		ENGINE_PROFILE_FUNCTION();
	}

	void VulkanTexture2D::Bind(uint32_t slot) const
	{
		ENGINE_PROFILE_FUNCTION();
	}

}

#include "enginepch.h"
#include "Platform/Vulkan/VulkanUniformBuffer.h"

namespace Engine
{
	VulkanUniformBuffer::VulkanUniformBuffer(uint32_t size, uint32_t binding)
	{
		ENGINE_PROFILE_FUNCTION();
	}

	VulkanUniformBuffer::~VulkanUniformBuffer()
	{
		ENGINE_PROFILE_FUNCTION();
	}

	void VulkanUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
	{
		ENGINE_PROFILE_FUNCTION();
	}
}

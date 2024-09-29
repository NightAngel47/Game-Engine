#include "enginepch.h"
#include "Platform/Vulkan/VulkanBuffer.h"

namespace Engine
{
	/////////////////////////////////////////////////////////////////////////////
	// VertexBuffer /////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	VulkanVertexBuffer::VulkanVertexBuffer(uint32_t size)
	{
		ENGINE_PROFILE_FUNCTION();
	}

	VulkanVertexBuffer::VulkanVertexBuffer(float* vertices, uint32_t size)
	{
		ENGINE_PROFILE_FUNCTION();
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		ENGINE_PROFILE_FUNCTION();
	}

	void VulkanVertexBuffer::Bind() const
	{
		ENGINE_PROFILE_FUNCTION();
	}

	void VulkanVertexBuffer::Unbind() const
	{
		ENGINE_PROFILE_FUNCTION();
	}

	void VulkanVertexBuffer::SetData(const void* data, uint32_t size)
	{
		ENGINE_PROFILE_FUNCTION();
	}

	/////////////////////////////////////////////////////////////////////////////
	// IndexBuffer //////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	VulkanIndexBuffer::VulkanIndexBuffer(uint32_t* indices, uint32_t count)
		:m_Count(count)
	{
		ENGINE_PROFILE_FUNCTION();
	}

	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
		ENGINE_PROFILE_FUNCTION();
	}

	void VulkanIndexBuffer::Bind() const
	{
		ENGINE_PROFILE_FUNCTION();
	}

	void VulkanIndexBuffer::Unbind() const
	{
		ENGINE_PROFILE_FUNCTION();
	}
}

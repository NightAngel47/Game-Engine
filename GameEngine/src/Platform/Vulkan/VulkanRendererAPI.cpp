#include "enginepch.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"

namespace Engine
{	
	void VulkanRendererAPI::Init()
	{
		ENGINE_PROFILE_FUNCTION();
	}

	void VulkanRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		ENGINE_PROFILE_FUNCTION();
	}

	void VulkanRendererAPI::SetClearColor(const glm::vec4& color)
	{
		ENGINE_PROFILE_FUNCTION();
	}

	void VulkanRendererAPI::Clear()
	{
		ENGINE_PROFILE_FUNCTION();
	}

	void VulkanRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		ENGINE_PROFILE_FUNCTION();
	}

	void VulkanRendererAPI::DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
	{
		ENGINE_PROFILE_FUNCTION();
	}

	void VulkanRendererAPI::SetLineWidth(float width)
	{
		ENGINE_PROFILE_FUNCTION();
	}

}

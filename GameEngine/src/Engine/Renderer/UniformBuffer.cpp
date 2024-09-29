#include "enginepch.h"

#include "Engine/Renderer/UniformBuffer.h"
#include "Engine/Renderer/Renderer.h"

//#include "Platform/OpenGL/OpenGLUniformBuffer.h"
#include "Platform/Vulkan/VulkanUniformBuffer.h"

namespace Engine
{
	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:		ENGINE_CORE_ASSERT(false, "RendererAPI::API::None is currently not supported!");  return nullptr;
			//case RendererAPI::API::OpenGL:		return CreateRef<OpenGLUniformBuffer>(size, binding);
			case RendererAPI::API::Vulkan:		return CreateRef<VulkanUniformBuffer>(size, binding);
		}

		ENGINE_CORE_ASSERT(false, "Unknown RendererAPI!")
		return nullptr;
	}
}

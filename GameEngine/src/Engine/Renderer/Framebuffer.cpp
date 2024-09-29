#include "enginepch.h"
#include "Engine/Renderer/Framebuffer.h"

#include "Engine/Renderer/Renderer.h"
//#include "Platform/OpenGL/OpenGLFramebuffer.h"
#include "Platform/Vulkan/VulkanFramebuffer.h"

namespace Engine
{
	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:		ENGINE_CORE_ASSERT(false, "RendererAPI::API::None is currently not supported!");  return nullptr;
			//case RendererAPI::API::OpenGL:		return CreateRef<OpenGLFramebuffer>(spec);
			case RendererAPI::API::Vulkan:		return CreateRef<VulkanFramebuffer>(spec);
		}

		ENGINE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
	
}

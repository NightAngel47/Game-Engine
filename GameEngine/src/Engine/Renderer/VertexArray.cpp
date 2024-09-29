#include "enginepch.h"
#include "Engine/Renderer/VertexArray.h"
		  
#include "Engine/Renderer/Renderer.h"
//#include "Platform/OpenGL/OpenGLVertexArray.h"
#include "Platform/Vulkan/VulkanVertexArray.h"

namespace Engine
{
	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:		ENGINE_CORE_ASSERT(false, "RendererAPI::API::None is currently not supported!");  return nullptr;
			//case RendererAPI::API::OpenGL:		return CreateRef<OpenGLVertexArray>();
			case RendererAPI::API::Vulkan:		return CreateRef<VulkanVertexArray>();
		}

		ENGINE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}

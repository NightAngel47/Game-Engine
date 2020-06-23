#include "enginepch.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Engine
{
	VertexArray* VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:		ENGINE_CORE_ASSERT(false, "RendererAPI::API::None is currently not supported!");  return nullptr;
			case RendererAPI::API::OpenGL:		return new OpenGLVertexArray();
		}

		ENGINE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
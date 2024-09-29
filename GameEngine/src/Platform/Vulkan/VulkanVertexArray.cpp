#include "enginepch.h"
#include "Platform/Vulkan/VulkanVertexArray.h"

namespace Engine
{
	VulkanVertexArray::VulkanVertexArray()
	{
		ENGINE_PROFILE_FUNCTION();
	}

	VulkanVertexArray::~VulkanVertexArray()
	{
		ENGINE_PROFILE_FUNCTION();
	}

	void VulkanVertexArray::Bind() const
	{
		ENGINE_PROFILE_FUNCTION();
	}

	void VulkanVertexArray::Unbind() const
	{
		ENGINE_PROFILE_FUNCTION();
	}

	void VulkanVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		ENGINE_PROFILE_FUNCTION();
		
		ENGINE_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

		vertexBuffer->Bind();
		
		const auto& layout =  vertexBuffer->GetLayout();
		for (const auto& element : layout)
		{
			switch (element.Type)
			{
				case ShaderDataType::Float:
				case ShaderDataType::Float2:
				case ShaderDataType::Float3:
				case ShaderDataType::Float4:
				{
					m_VertexBufferIndex++;
					break;
				}
				case ShaderDataType::Int:
				case ShaderDataType::Int2:
				case ShaderDataType::Int3:
				case ShaderDataType::Int4:
				case ShaderDataType::Bool:
				{
					m_VertexBufferIndex++;
					break;
				}
				case ShaderDataType::Mat3:
				case ShaderDataType::Mat4:
				{
					uint8_t count = element.GetComponentCount();
					for (uint8_t i = 0; i < count; i++)
					{
						m_VertexBufferIndex++;
					}
					break;
				}
				default:
					ENGINE_CORE_ASSERT(false, "Unknown ShaderDataType!");
			}
		}

		m_VertexBuffers.push_back(vertexBuffer);
	}

	void VulkanVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		ENGINE_PROFILE_FUNCTION();
		
		indexBuffer->Bind();
		
		m_IndexBuffer = indexBuffer;
	}
}

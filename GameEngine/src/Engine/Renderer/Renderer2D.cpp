#include "enginepch.h"
#include "Engine/Renderer/Renderer2D.h"

#include "Engine/Renderer/VertexArray.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Renderer/RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Engine
{
	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexureIndex;
		float TilingFactor;
	};
	
	struct Render2DData
	{
		static const uint32_t MaxQuads = 10000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32; // TODO: RenderCaps
		
		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> TextureShader;
		Ref<Texture2D> WhiteTexture;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 = white texture

		glm::vec4 QuadVertexPositions[4];

		Renderer2D::Statistics Stats;
	};

	static Render2DData s_Data;
	
	void Renderer2D::Init()
	{
		ENGINE_PROFILE_FUNCTION();
		
		s_Data.QuadVertexArray = VertexArray::Create();

		s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));
		s_Data.QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float, "a_TexureIndex" },
			{ ShaderDataType::Float, "a_TilingFactor" }
		});
		s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

		s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];

		uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];
		
		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;
			
			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_Data.MaxIndices);
		s_Data.QuadVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices;

		s_Data.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		int32_t samplers[s_Data.MaxTextureSlots];
		for (uint32_t i = 0; i < s_Data.MaxTextureSlots; i++)
			samplers[i] = i;
		
		s_Data.TextureShader = Shader::Create("assets/shaders/Texture.glsl");
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetIntArray("u_Textures", samplers, s_Data.MaxTextureSlots);

		s_Data.TextureSlots[0] = s_Data.WhiteTexture;

		s_Data.QuadVertexPositions[0] = {-0.5f, -0.5f, 0.0f, 1.0f};
		s_Data.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f};
		s_Data.QuadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f};
		s_Data.QuadVertexPositions[3] = {-0.5f,  0.5f, 0.0f, 1.0f};
	}

	void Renderer2D::Shutdown()
	{
		ENGINE_PROFILE_FUNCTION();
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		ENGINE_PROFILE_FUNCTION();
		
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		s_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::EndScene()
	{
		ENGINE_PROFILE_FUNCTION();

		uint32_t dataSize = (uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase;
		s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);
		
		Flush();
	}

	void Renderer2D::Flush()
	{
		ENGINE_PROFILE_FUNCTION();

		for (uint32_t i =0; i < s_Data.TextureSlotIndex; i++)
			s_Data.TextureSlots[i]->Bind(i);

		RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);

		s_Data.Stats.DrawCalls++;
	}

	void Renderer2D::FlushAndReset()
	{
		ENGINE_PROFILE_FUNCTION();
		
		EndScene();
		
		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		s_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const float& rotation, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({position.x, position.y, 0.0f}, rotation, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const float& rotation, const glm::vec2& size, const glm::vec4& color)
	{
		ENGINE_PROFILE_FUNCTION();
		
		SetQuadVertexBuffer(GenTransform(position, rotation, size), color, 0.0f, 1.0f);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const float& rotation, const glm::vec2& size, const Ref<Texture2D>& texture, const float& tiling, const glm::vec4& color)
	{
		DrawQuad({position.x, position.y, 0.0f}, rotation, size, texture, tiling, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const float& rotation, const glm::vec2& size, const Ref<Texture2D>& texture, const float& tiling, const glm::vec4& color)
	{
		ENGINE_PROFILE_FUNCTION();
		
		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (*s_Data.TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}
		
		if (textureIndex == 0.0f)
		{
			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}
		
		SetQuadVertexBuffer(GenTransform(position, rotation, size), color, textureIndex, tiling);
	}

	glm::mat4 Renderer2D::GenTransform(const glm::vec3& position, const float& rotation, const glm::vec2& size)
	{
		ENGINE_PROFILE_FUNCTION();
		
		glm::mat4 transform;
		
		if (rotation)
		{	
			transform = glm::translate(glm::mat4(1.0f), position) * 
				glm::rotate(glm::mat4(1.0f), rotation, {0.0f, 0.0f, 1.0f}) * 
				glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});
		}
		else
		{
			transform = glm::translate(glm::mat4(1.0f), position) * 
				glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});
		}
		
		return transform;
	}

	void Renderer2D::SetQuadVertexBuffer(const glm::mat4& transfrom, const glm::vec4& color, const float& textureIndex, const float& tiling)
	{
		ENGINE_PROFILE_FUNCTION();

		if (s_Data.QuadIndexCount >= Render2DData::MaxIndices)
			FlushAndReset();
		
		for (uint32_t i = 0; i < 4; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transfrom * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			
			switch (i)
			{
			case 0:
				s_Data.QuadVertexBufferPtr->TexCoord = {0.0f, 0.0f};
				break;
			case 1:
				s_Data.QuadVertexBufferPtr->TexCoord = {1.0f, 0.0f};
				break;
			case 2:
				s_Data.QuadVertexBufferPtr->TexCoord = {1.0f, 1.0f};
				break;
			case 3:
				s_Data.QuadVertexBufferPtr->TexCoord = {0.0f, 1.0f};
				break;
			}
			
			s_Data.QuadVertexBufferPtr->TexureIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tiling;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::ResetStats()
	{
		memset(&s_Data.Stats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data.Stats;
	}
	
}

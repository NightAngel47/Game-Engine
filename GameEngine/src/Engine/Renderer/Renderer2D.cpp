#include "enginepch.h"
#include "Engine/Renderer/Renderer2D.h"

#include "Engine/Renderer/VertexArray.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/Renderer/UniformBuffer.h"
#include "Engine/Renderer/RenderCommand.h"
#include "Engine/Renderer/MSDFData.h"
#include "Engine/Math/Math.h"

#include <glm/gtc/matrix_transform.hpp>


namespace Engine
{
	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TilingFactor;
		float TexureIndex;

		// Editor-only
		int EntityID;
	};

	struct CircleVertex
	{
		glm::vec3 WorldPosition;
		glm::vec2 LocalPosition;
		glm::vec4 Color;
		float Thickness;
		float Fade;

		// Editor-only
		int EntityID;
	};

	struct LineVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;

		// Editor-only
		int EntityID;
	};

	struct TextVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;

		// TODO: bg color for outline/bg

		// Editor-only
		int EntityID;
	};
	
	struct Render2DData
	{
		static const uint32_t MaxQuads = 10000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32; // TODO: RenderCaps
		
		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> QuadShader;
		Ref<Texture2D> WhiteTexture;

		Ref<VertexArray> CircleVertexArray;
		Ref<VertexBuffer> CircleVertexBuffer;
		Ref<Shader> CircleShader;

		Ref<VertexArray> LineVertexArray;
		Ref<VertexBuffer> LineVertexBuffer;
		Ref<Shader> LineShader;

		Ref<VertexArray> TextVertexArray;
		Ref<VertexBuffer> TextVertexBuffer;
		Ref<Shader> TextShader;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		uint32_t CircleIndexCount = 0;
		CircleVertex* CircleVertexBufferBase = nullptr;
		CircleVertex* CircleVertexBufferPtr = nullptr;

		uint32_t LineVertexCount = 0;
		LineVertex* LineVertexBufferBase = nullptr;
		LineVertex* LineVertexBufferPtr = nullptr;

		uint32_t TextIndexCount = 0;
		TextVertex* TextVertexBufferBase = nullptr;
		TextVertex* TextVertexBufferPtr = nullptr;

		float LineWidth = 2.0f;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 = white texture

		Ref<Texture2D> FontAtlasTexture;

		glm::vec4 QuadVertexPositions[4];

		Renderer2D::Statistics Stats;

		struct CameraData
		{
			glm::mat4 ViewProjection;
		};

		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUniformBuffer;
	};

	static Render2DData s_Renderer2DData;
	
	void Renderer2D::Init()
	{
		ENGINE_PROFILE_FUNCTION();
		
		// Quads
		s_Renderer2DData.QuadVertexArray = VertexArray::Create();

		s_Renderer2DData.QuadVertexBuffer = VertexBuffer::Create(s_Renderer2DData.MaxVertices * sizeof(QuadVertex));
		s_Renderer2DData.QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Float3,	"a_Position"		},
			{ ShaderDataType::Float4,	"a_Color"			},
			{ ShaderDataType::Float2,	"a_TexCoord"		},
			{ ShaderDataType::Float,	"a_TilingFactor"	},
			{ ShaderDataType::Float,	"a_TexIndex"		},
			{ ShaderDataType::Int,		"a_EntityID"		}
		});
		s_Renderer2DData.QuadVertexArray->AddVertexBuffer(s_Renderer2DData.QuadVertexBuffer);
		s_Renderer2DData.QuadVertexBufferBase = new QuadVertex[s_Renderer2DData.MaxVertices];
		
		uint32_t* quadIndices = new uint32_t[s_Renderer2DData.MaxIndices];
		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Renderer2DData.MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;
			
			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_Renderer2DData.MaxIndices);
		s_Renderer2DData.QuadVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices;

		// Circles
		s_Renderer2DData.CircleVertexArray = VertexArray::Create();

		s_Renderer2DData.CircleVertexBuffer = VertexBuffer::Create(s_Renderer2DData.MaxVertices * sizeof(CircleVertex));
		s_Renderer2DData.CircleVertexBuffer->SetLayout({
			{ ShaderDataType::Float3,	"a_WorldPosition"	},
			{ ShaderDataType::Float2,	"a_LocalPosition"	},
			{ ShaderDataType::Float4,	"a_Color"			},
			{ ShaderDataType::Float,	"a_Thickness"		},
			{ ShaderDataType::Float,	"a_Fade"			},
			{ ShaderDataType::Int,		"a_EntityID"		}
			});
		s_Renderer2DData.CircleVertexArray->AddVertexBuffer(s_Renderer2DData.CircleVertexBuffer);
		s_Renderer2DData.CircleVertexBufferBase = new CircleVertex[s_Renderer2DData.MaxVertices];
		s_Renderer2DData.CircleVertexArray->SetIndexBuffer(quadIB); // Use quad IB (identical implementation otherwise)

		// Lines
		s_Renderer2DData.LineVertexArray = VertexArray::Create();

		s_Renderer2DData.LineVertexBuffer = VertexBuffer::Create(s_Renderer2DData.MaxVertices * sizeof(LineVertex));
		s_Renderer2DData.LineVertexBuffer->SetLayout({
			{ ShaderDataType::Float3,	"a_Position"		},
			{ ShaderDataType::Float4,	"a_Color"			},
			{ ShaderDataType::Int,		"a_EntityID"		}
			});
		s_Renderer2DData.LineVertexArray->AddVertexBuffer(s_Renderer2DData.LineVertexBuffer);
		s_Renderer2DData.LineVertexBufferBase = new LineVertex[s_Renderer2DData.MaxVertices];

		// Text
		s_Renderer2DData.TextVertexArray = VertexArray::Create();

		s_Renderer2DData.TextVertexBuffer = VertexBuffer::Create(s_Renderer2DData.MaxVertices * sizeof(TextVertex));
		s_Renderer2DData.TextVertexBuffer->SetLayout({
			{ ShaderDataType::Float3,	"a_Position"		},
			{ ShaderDataType::Float4,	"a_Color"			},
			{ ShaderDataType::Float2,	"a_TexCoord"		},
			{ ShaderDataType::Int,		"a_EntityID"		}
			});
		s_Renderer2DData.TextVertexArray->AddVertexBuffer(s_Renderer2DData.TextVertexBuffer);
		s_Renderer2DData.TextVertexBufferBase = new TextVertex[s_Renderer2DData.MaxVertices];
		s_Renderer2DData.TextVertexArray->SetIndexBuffer(quadIB); // Use quad IB (identical implementation otherwise)

		uint32_t whiteTextureData = 0xffffffff;
		s_Renderer2DData.WhiteTexture = Texture2D::Create(TextureSpecification(), Buffer(&whiteTextureData, sizeof(uint32_t)));

		int32_t samplers[s_Renderer2DData.MaxTextureSlots];
		for (uint32_t i = 0; i < s_Renderer2DData.MaxTextureSlots; i++)
			samplers[i] = i;
		
		s_Renderer2DData.QuadShader = Shader::Create("assets/shaders/Renderer2D_Quad.glsl");
		s_Renderer2DData.CircleShader = Shader::Create("assets/shaders/Renderer2D_Circle.glsl");
		s_Renderer2DData.LineShader = Shader::Create("assets/shaders/Renderer2D_Line.glsl");
		s_Renderer2DData.TextShader = Shader::Create("assets/shaders/Renderer2D_Text.glsl");

		// Set first texture to slot 0
		s_Renderer2DData.TextureSlots[0] = s_Renderer2DData.WhiteTexture;

		s_Renderer2DData.QuadVertexPositions[0] = {-0.5f, -0.5f, 0.0f, 1.0f};
		s_Renderer2DData.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f};
		s_Renderer2DData.QuadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f};
		s_Renderer2DData.QuadVertexPositions[3] = {-0.5f,  0.5f, 0.0f, 1.0f};

		s_Renderer2DData.CameraUniformBuffer = UniformBuffer::Create(sizeof(Render2DData::CameraData), 0);
	}

	void Renderer2D::Shutdown()
	{
		ENGINE_PROFILE_FUNCTION();
	}

	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		ENGINE_PROFILE_FUNCTION();
		
		s_Renderer2DData.CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);
		s_Renderer2DData.CameraUniformBuffer->SetData(&s_Renderer2DData.CameraBuffer, sizeof(Render2DData::CameraData));
		
		StartBatch();
	}

	void Renderer2D::BeginScene(const EditorCamera& camera)
	{
		ENGINE_PROFILE_FUNCTION();
		
		s_Renderer2DData.CameraBuffer.ViewProjection = camera.GetViewProjection();
		s_Renderer2DData.CameraUniformBuffer->SetData(&s_Renderer2DData.CameraBuffer, sizeof(Render2DData::CameraData));
		
		StartBatch();
	}

	void Renderer2D::EndScene()
	{
		ENGINE_PROFILE_FUNCTION();
		
		Flush();
	}

	void Renderer2D::StartBatch()
	{
		s_Renderer2DData.QuadIndexCount = 0;
		s_Renderer2DData.QuadVertexBufferPtr = s_Renderer2DData.QuadVertexBufferBase;

		s_Renderer2DData.CircleIndexCount = 0;
		s_Renderer2DData.CircleVertexBufferPtr = s_Renderer2DData.CircleVertexBufferBase;

		s_Renderer2DData.LineVertexCount = 0;
		s_Renderer2DData.LineVertexBufferPtr = s_Renderer2DData.LineVertexBufferBase;

		s_Renderer2DData.TextIndexCount = 0;
		s_Renderer2DData.TextVertexBufferPtr = s_Renderer2DData.TextVertexBufferBase;

		s_Renderer2DData.TextureSlotIndex = 1;
	}

	void Renderer2D::Flush()
	{
		ENGINE_PROFILE_FUNCTION();

		if (s_Renderer2DData.QuadIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Renderer2DData.QuadVertexBufferPtr - (uint8_t*)s_Renderer2DData.QuadVertexBufferBase);
			s_Renderer2DData.QuadVertexBuffer->SetData(s_Renderer2DData.QuadVertexBufferBase, dataSize);

			for (uint32_t i = 0; i < s_Renderer2DData.TextureSlotIndex; i++)
				s_Renderer2DData.TextureSlots[i]->Bind(i);

			s_Renderer2DData.QuadShader->Bind();
			RenderCommand::DrawIndexed(s_Renderer2DData.QuadVertexArray, s_Renderer2DData.QuadIndexCount);

			s_Renderer2DData.Stats.DrawCalls++;
		}

		if (s_Renderer2DData.CircleIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Renderer2DData.CircleVertexBufferPtr - (uint8_t*)s_Renderer2DData.CircleVertexBufferBase);
			s_Renderer2DData.CircleVertexBuffer->SetData(s_Renderer2DData.CircleVertexBufferBase, dataSize);

			s_Renderer2DData.CircleShader->Bind();
			RenderCommand::DrawIndexed(s_Renderer2DData.CircleVertexArray, s_Renderer2DData.CircleIndexCount);

			s_Renderer2DData.Stats.DrawCalls++;
		}

		if (s_Renderer2DData.LineVertexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Renderer2DData.LineVertexBufferPtr - (uint8_t*)s_Renderer2DData.LineVertexBufferBase);
			s_Renderer2DData.LineVertexBuffer->SetData(s_Renderer2DData.LineVertexBufferBase, dataSize);

			s_Renderer2DData.LineShader->Bind();
			RenderCommand::SetLineWidth(s_Renderer2DData.LineWidth);
			RenderCommand::DrawLines(s_Renderer2DData.LineVertexArray, s_Renderer2DData.LineVertexCount);

			s_Renderer2DData.Stats.DrawCalls++;
		}

		if (s_Renderer2DData.TextIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Renderer2DData.TextVertexBufferPtr - (uint8_t*)s_Renderer2DData.TextVertexBufferBase);
			s_Renderer2DData.TextVertexBuffer->SetData(s_Renderer2DData.TextVertexBufferBase, dataSize);

			s_Renderer2DData.FontAtlasTexture->Bind();

			s_Renderer2DData.TextShader->Bind();
			RenderCommand::DrawIndexed(s_Renderer2DData.TextVertexArray, s_Renderer2DData.TextIndexCount);

			s_Renderer2DData.Stats.DrawCalls++;
		}
	}

	void Renderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const float rotation, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({position.x, position.y, 0.0f}, rotation, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const float rotation, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad(Math::GenRectTransform(position, rotation, size), color);
	}

	//void Renderer2D::DrawQuad(const glm::vec2& position, const float rotation, const glm::vec2& size, const Ref<Texture2D>& texture, const float tiling, const glm::vec4& color)
	void Renderer2D::DrawQuad(const glm::vec2& position, const float rotation, const glm::vec2& size, const AssetHandle texture, const float tiling, const glm::vec4& color)
	{
		DrawQuad({position.x, position.y, 0.0f}, rotation, size, texture, tiling, color);
	}

	//void Renderer2D::DrawQuad(const glm::vec3& position, const float rotation, const glm::vec2& size, const Ref<Texture2D>& texture, const float tiling, const glm::vec4& color)
	void Renderer2D::DrawQuad(const glm::vec3& position, const float rotation, const glm::vec2& size, const AssetHandle texture, const float tiling, const glm::vec4& color)
	{
		DrawQuad(Math::GenRectTransform(position, rotation, size), texture, tiling, color);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const float rotation, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, const float tiling, const glm::vec4& color)
	{
		DrawQuad({position.x, position.y, 0.0f}, rotation, size, subtexture, tiling, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const float rotation, const glm::vec2& size, const Ref<SubTexture2D>& subtexture, const float tiling, const glm::vec4& color)
	{
		
		DrawQuad(Math::GenRectTransform(position, rotation, size), subtexture, tiling, color);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		ENGINE_PROFILE_FUNCTION();

		constexpr glm::vec2 textureCoords[] = {{ 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f }};
		
		SetQuadVertexBuffer(transform, color, textureCoords, 0.0f, 1.0f, entityID);
	}

	//void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, const float tiling, const glm::vec4& color, int entityID)
	void Renderer2D::DrawQuad(const glm::mat4& transform, const AssetHandle texture, const float tiling, const glm::vec4& color, int entityID)
	{
		ENGINE_PROFILE_FUNCTION();
		
		constexpr glm::vec2 textureCoords[] = {{ 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f }};
		
		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_Renderer2DData.TextureSlotIndex; i++)
		{
			if (s_Renderer2DData.TextureSlots[i]->Handle == texture)
			{
				textureIndex = (float)i;
				break;
			}
		}
		
		if (textureIndex == 0.0f)
		{
			if (s_Renderer2DData.TextureSlotIndex >= Render2DData::MaxTextureSlots)
				Flush();
			
			textureIndex = (float)s_Renderer2DData.TextureSlotIndex;
			Ref<Texture2D> texture2D = AssetManager::GetAsset<Texture2D>(texture);
			ENGINE_CORE_VERIFY(texture2D);
			s_Renderer2DData.TextureSlots[s_Renderer2DData.TextureSlotIndex] = texture2D;
			s_Renderer2DData.TextureSlotIndex++;
		}
		
		SetQuadVertexBuffer(transform, color, textureCoords, textureIndex, tiling, entityID);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<SubTexture2D>& subtexture, const float tiling, const glm::vec4& color, int entityID)
	{
		ENGINE_PROFILE_FUNCTION();

		const AssetHandle texture = subtexture->GetTexture();
		const glm::vec2* textureCoords = subtexture->GetTexCoords();
		
		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_Renderer2DData.TextureSlotIndex; i++)
		{
			if (s_Renderer2DData.TextureSlots[i]->Handle == texture)
			{
				textureIndex = (float)i;
				break;
			}
		}
		
		if (textureIndex == 0.0f)
		{
			if (s_Renderer2DData.TextureSlotIndex >= Render2DData::MaxTextureSlots)
				Flush();
			
			textureIndex = (float)s_Renderer2DData.TextureSlotIndex;
			s_Renderer2DData.TextureSlots[s_Renderer2DData.TextureSlotIndex] = AssetManager::GetAsset<Texture2D>(texture);
			s_Renderer2DData.TextureSlotIndex++;
		}
		
		SetQuadVertexBuffer(transform, color, textureCoords, textureIndex, tiling, entityID);
	}

	void Renderer2D::DrawCircle(const glm::mat4& transform, const glm::vec4& color, const float thickness, const float fade, int entityID)
	{
		ENGINE_PROFILE_FUNCTION();

		SetCircleVertexBuffer(transform, color, thickness, fade, entityID);
	}

	void Renderer2D::DrawLine(const glm::vec3& pos0, const glm::vec3& pos1, const glm::vec4& color, int entityID)
	{
		ENGINE_PROFILE_FUNCTION();

		s_Renderer2DData.LineVertexBufferPtr->Position = pos0;
		s_Renderer2DData.LineVertexBufferPtr->Color = color;
		s_Renderer2DData.LineVertexBufferPtr->EntityID = entityID;
		s_Renderer2DData.LineVertexBufferPtr++;

		s_Renderer2DData.LineVertexBufferPtr->Position = pos1;
		s_Renderer2DData.LineVertexBufferPtr->Color = color;
		s_Renderer2DData.LineVertexBufferPtr->EntityID = entityID;
		s_Renderer2DData.LineVertexBufferPtr++;

		s_Renderer2DData.LineVertexCount += 2;
	}

	void Renderer2D::DrawRect(const glm::vec3& position, const float rotation, const glm::vec2& size, const glm::vec4& color, int entityID)
	{
		DrawRect(Math::GenRectTransform(position, rotation, size), color, entityID);
	}

	void Renderer2D::DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		glm::vec3 lineVertices[4];
		for (size_t i = 0; i < 4; ++i)
		{
			lineVertices[i] = transform * s_Renderer2DData.QuadVertexPositions[i];
		}

		DrawLine(lineVertices[0], lineVertices[1], color, entityID);
		DrawLine(lineVertices[1], lineVertices[2], color, entityID);
		DrawLine(lineVertices[2], lineVertices[3], color, entityID);
		DrawLine(lineVertices[3], lineVertices[0], color, entityID);
	}

	void Renderer2D::DrawSprite(const glm::mat4& transform, SpriteRendererComponent& src, int entityID)
	{
		ENGINE_PROFILE_FUNCTION();

		if (src.Texture.IsValid())
		{
			if (src.IsSubTexture)
			{
				DrawQuad(transform, src.SubTexture, src.Tiling, src.Color, entityID);
			}
			else
			{
				DrawQuad(transform, src.Texture, src.Tiling, src.Color, entityID);
			}
		}
		else
		{
			DrawQuad(transform, src.Color, entityID);
		}
	}

	void Renderer2D::DrawString(const std::string& string, const glm::mat4& transform, const TextParams& textParams, int entityID)
	{
		ENGINE_PROFILE_FUNCTION();

		const auto& fontGeo = textParams.Font->GetMSDFData()->FontGeo;
		const auto& metrics = fontGeo.getMetrics();
		s_Renderer2DData.FontAtlasTexture = textParams.Font->GetAtlasTexture();

		double x = 0.0;
		double fsScale = 1.0 / (metrics.ascenderY - metrics.descenderY);
		double y = 0.0;

		const float spaceGlyphAdvance = fontGeo.getGlyph(' ')->getAdvance();

		for (size_t i = 0; i < string.size(); ++i)
		{
			char character = string[i];

			// handle unique characters
			switch (character)
			{
				case '\r':
					continue;
				case '\n':
				{
					x = 0;
					y -= fsScale * metrics.lineHeight + textParams.LineSpacing;
					continue;
				}
				case ' ':
				{
					if (i < string.size() - 1)
					{
						char nextCharacter = string[i + 1];
						double advance;
						fontGeo.getAdvance(advance, character, nextCharacter);

						x += fsScale * advance + textParams.Kerning;
					}
					continue;
				}
				case '\t':
					x += 4.0f * (fsScale * spaceGlyphAdvance + textParams.Kerning);
					continue;
			}

			auto glyph = fontGeo.getGlyph(character);
			if (!glyph)
				glyph = fontGeo.getGlyph('?'); // missing character
			if (!glyph)
				return; //continue; // failsafe, missing character

			double al, ab, ar, at;
			glyph->getQuadAtlasBounds(al, ab, ar, at);
			glm::vec2 texCoordMin((float)al, (float)ab);
			glm::vec2 texCoordMax((float)ar, (float)at);

			double pl, pb, pr, pt;
			glyph->getQuadPlaneBounds(pl, pb, pr, pt);
			glm::vec2 quadMin((float)pl, (float)pb);
			glm::vec2 quadMax((float)pr, (float)pt);

			quadMin *= fsScale;
			quadMax *= fsScale;
			quadMin += glm::vec2(x, y);
			quadMax += glm::vec2(x, y);

			float texelWidth = 1.0f / s_Renderer2DData.FontAtlasTexture->GetWidth();
			float texelHeight = 1.0f / s_Renderer2DData.FontAtlasTexture->GetHeight();
			texCoordMin *= glm::vec2(texelWidth, texelHeight);
			texCoordMax *= glm::vec2(texelWidth, texelHeight);

			// render here

			s_Renderer2DData.TextVertexBufferPtr->Position = transform * glm::vec4(quadMin, 0.0f, 1.0f);
			s_Renderer2DData.TextVertexBufferPtr->Color = textParams.Color;
			s_Renderer2DData.TextVertexBufferPtr->TexCoord = texCoordMin;
			s_Renderer2DData.TextVertexBufferPtr->EntityID = entityID;
			s_Renderer2DData.TextVertexBufferPtr++;

			s_Renderer2DData.TextVertexBufferPtr->Position = transform * glm::vec4(quadMin.x, quadMax.y, 0.0f, 1.0f);
			s_Renderer2DData.TextVertexBufferPtr->Color = textParams.Color;
			s_Renderer2DData.TextVertexBufferPtr->TexCoord = { texCoordMin.x, texCoordMax.y };
			s_Renderer2DData.TextVertexBufferPtr->EntityID = entityID;
			s_Renderer2DData.TextVertexBufferPtr++;

			s_Renderer2DData.TextVertexBufferPtr->Position = transform * glm::vec4(quadMax, 0.0f, 1.0f);
			s_Renderer2DData.TextVertexBufferPtr->Color = textParams.Color;
			s_Renderer2DData.TextVertexBufferPtr->TexCoord = texCoordMax;
			s_Renderer2DData.TextVertexBufferPtr->EntityID = entityID;
			s_Renderer2DData.TextVertexBufferPtr++;

			s_Renderer2DData.TextVertexBufferPtr->Position = transform * glm::vec4(quadMax.x, quadMin.y, 0.0f, 1.0f);
			s_Renderer2DData.TextVertexBufferPtr->Color = textParams.Color;
			s_Renderer2DData.TextVertexBufferPtr->TexCoord = { texCoordMax.x, texCoordMin.y };
			s_Renderer2DData.TextVertexBufferPtr->EntityID = entityID;
			s_Renderer2DData.TextVertexBufferPtr++;

			s_Renderer2DData.TextIndexCount += 6;
			s_Renderer2DData.Stats.QuadCount++;

			if (i < string.size() - 1)
			{
				double advance = glyph->getAdvance();
				char nextCharacter = string[i + 1];
				fontGeo.getAdvance(advance, character, nextCharacter);

				x += fsScale * advance + textParams.Kerning;
			}
		}
	}

	void Renderer2D::DrawString(const std::string& string, const glm::mat4& transform, TextRendererComponent& trc, int entityID)
	{
		TextParams textParams{ trc.FontAsset, trc.Color, trc.Kerning, trc.LineSpacing };
		DrawString(string, transform, textParams, entityID);
	}

	float Renderer2D::GetLineWidth()
	{
		return s_Renderer2DData.LineWidth;
	}

	void Renderer2D::SetLineWidth(float width)
	{
		s_Renderer2DData.LineWidth = width;
	}

	void Renderer2D::SetQuadVertexBuffer(const glm::mat4& transform, const glm::vec4& color,  const glm::vec2* textureCoords, const float textureIndex, const float tiling, int entityID)
	{
		ENGINE_PROFILE_FUNCTION();

		if (s_Renderer2DData.QuadIndexCount >= Render2DData::MaxIndices)
			NextBatch();
		
		for (uint32_t i = 0; i < 4; i++)
		{
			s_Renderer2DData.QuadVertexBufferPtr->Position = transform * s_Renderer2DData.QuadVertexPositions[i];
			s_Renderer2DData.QuadVertexBufferPtr->Color = color;
			s_Renderer2DData.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Renderer2DData.QuadVertexBufferPtr->TilingFactor = tiling;
			s_Renderer2DData.QuadVertexBufferPtr->TexureIndex = textureIndex;
			s_Renderer2DData.QuadVertexBufferPtr->EntityID = entityID;
			s_Renderer2DData.QuadVertexBufferPtr++;
		}

		s_Renderer2DData.QuadIndexCount += 6;
		s_Renderer2DData.Stats.QuadCount++;
	}

	void Renderer2D::SetCircleVertexBuffer(const glm::mat4& transform, const glm::vec4& color, const float thickness, const float fade, int entityID)
	{
		ENGINE_PROFILE_FUNCTION();

		if (s_Renderer2DData.CircleIndexCount >= Render2DData::MaxIndices)
			NextBatch(); // TODO: Create separate flushes for circles and quads

		for (uint32_t i = 0; i < 4; i++)
		{
			s_Renderer2DData.CircleVertexBufferPtr->WorldPosition = transform * s_Renderer2DData.QuadVertexPositions[i];
			s_Renderer2DData.CircleVertexBufferPtr->LocalPosition = s_Renderer2DData.QuadVertexPositions[i] * 2.0f;
			s_Renderer2DData.CircleVertexBufferPtr->Color = color;
			s_Renderer2DData.CircleVertexBufferPtr->Thickness = thickness;
			s_Renderer2DData.CircleVertexBufferPtr->Fade = fade;
			s_Renderer2DData.CircleVertexBufferPtr->EntityID = entityID;
			s_Renderer2DData.CircleVertexBufferPtr++;
		}

		s_Renderer2DData.CircleIndexCount += 6;
		s_Renderer2DData.Stats.QuadCount++;
	}

	void Renderer2D::ResetStats()
	{
		memset(&s_Renderer2DData.Stats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Renderer2DData.Stats;
	}
	
}

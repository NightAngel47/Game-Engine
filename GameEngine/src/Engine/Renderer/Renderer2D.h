#pragma once

#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/EditorCamera.h"
#include "Engine/Renderer/OrthographicCamera.h"
		  
#include "Engine/Renderer/Texture.h"
#include "Engine/Renderer/SubTexture2D.h"
#include "Engine/Renderer/Font.h"
#include "Engine/Scene/Components.h"

namespace Engine
{
	
	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();
		
		static void BeginScene(const Camera& camera, const glm::mat4& transform);
		static void BeginScene(const EditorCamera& camera);
		static void BeginScene(const OrthographicCamera& camera); // TODO: Remove
		static void EndScene();
		static void Flush();

		// Primitives
		static void DrawQuad(const glm::vec2& position = glm::vec2(0.0f), const float rotation = 0.0f, const glm::vec2& size = glm::vec2(1.0f), const glm::vec4& color = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec3& position = glm::vec3(0.0f), const float rotation = 0.0f, const glm::vec2& size = glm::vec2(1.0f), const glm::vec4& color = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec2& position = glm::vec2(0.0f), const float rotation = 0.0f, const glm::vec2& size = glm::vec2(1.0f), const Ref<Texture2D>& texture = nullptr, const float tiling = 1.0f, const glm::vec4& color = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec3& position = glm::vec3(0.0f), const float rotation = 0.0f, const glm::vec2& size = glm::vec2(1.0f), const Ref<Texture2D>& texture = nullptr, const float tiling = 1.0f, const glm::vec4& color = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec2& position = glm::vec2(0.0f), const float rotation = 0.0f, const glm::vec2& size = glm::vec2(1.0f), const Ref<SubTexture2D>& subtexture = nullptr, const float tiling = 1.0f, const glm::vec4& color = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec3& position = glm::vec3(0.0f), const float rotation = 0.0f, const glm::vec2& size = glm::vec2(1.0f), const Ref<SubTexture2D>& subtexture = nullptr, const float tiling = 1.0f, const glm::vec4& color = glm::vec4(1.0f));

		static void DrawQuad(const glm::mat4& transform = glm::mat4(1.0f), const glm::vec4& color = glm::vec4(1.0f), int entityID = -1);
		static void DrawQuad(const glm::mat4& transform = glm::mat4(1.0f), const Ref<Texture2D>& texture = nullptr, const float tiling = 1.0f, const glm::vec4& color = glm::vec4(1.0f), int entityID = -1);
		static void DrawQuad(const glm::mat4& transform = glm::mat4(1.0f), const Ref<SubTexture2D>& subtexture = nullptr, const float tiling = 1.0f, const glm::vec4& color = glm::vec4(1.0f), int entityID = -1);
		
		static void DrawCircle(const glm::mat4& transform = glm::mat4(1.0f), const glm::vec4& color = glm::vec4(1.0f), const float thickness = 1.0f, const float fade = 0.005f, int entityID = -1);

		static void DrawLine(const glm::vec3& pos0, const glm::vec3& pos1, const glm::vec4& color = glm::vec4(1.0f), int entityID = -1);
		static void DrawRect(const glm::vec3& position = glm::vec3(0.0f), const float rotation = 0.0f, const glm::vec2& size = glm::vec2(1.0f), const glm::vec4& color = glm::vec4(1.0f), int entityID = -1);
		static void DrawRect(const glm::mat4& transform = glm::mat4(1.0f), const glm::vec4& color = glm::vec4(1.0f), int entityID = -1);

		static void DrawSprite(const glm::mat4& transform, SpriteRendererComponent& src, int entityID);
		static void DrawUIImage(const glm::mat4& transform, UIImageComponent& uiImage, int entityID);

		struct TextParams
		{
			Ref<Font> Font = Font::GetDefault();
			glm::vec4 Color{ 1.0f };

			float Kerning = 0.0f;
			float LineSpacing = 0.0f;
		};
		static void DrawString(const std::string& string, const glm::mat4& transform, const TextParams& textParams, int entityID = -1);
		static void DrawString(const std::string& string, const glm::mat4& transform, TextRendererComponent& trc, int entityID = -1);

		static float GetLineWidth();
		static void SetLineWidth(float width);
		
		// Stats
		struct Statistics
		{
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;
			
			uint32_t GetTotalVertexCount() const { return QuadCount * 4; }
			uint32_t GetTotalIndexCount() const { return QuadCount * 6; }
		};
		
		static Statistics GetStats();
		static void ResetStats();
	private:
		static void StartBatch();
		static void NextBatch();
		static void SetQuadVertexBuffer(const glm::mat4& transfrom, const glm::vec4& color, const glm::vec2* textureCoords, const float textureIndex, const float tiling, int entityID);
		static void SetCircleVertexBuffer(const glm::mat4& transfrom, const glm::vec4& color, const float thickness, const float fade, int entityID);
	};
	
}

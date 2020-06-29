#pragma once

#include "Engine/Renderer/OrthographicCamera.h"
		  
#include "Engine/Renderer/Texture.h"

namespace Engine
{
	
	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();
		
		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();
		static void Flush();

		// Primitives
		static void DrawQuad(const glm::vec2& position = glm::vec2(0.0f), const float& rotation = 0.0f, const glm::vec2& size = glm::vec2(1.0f), const glm::vec4& color = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec3& position = glm::vec3(0.0f), const float& rotation = 0.0f, const glm::vec2& size = glm::vec2(1.0f), const glm::vec4& color = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec2& position = glm::vec2(0.0f), const float& rotation = 0.0f, const glm::vec2& size = glm::vec2(1.0f), const Ref<Texture2D>& texture = nullptr, const float& tiling = 1.0f, const glm::vec4& color = glm::vec4(1.0f));
		static void DrawQuad(const glm::vec3& position = glm::vec3(0.0f), const float& rotation = 0.0f, const glm::vec2& size = glm::vec2(1.0f), const Ref<Texture2D>& texture = nullptr, const float& tiling = 1.0f, const glm::vec4& color = glm::vec4(1.0f));
	private:
		static glm::mat4 GenTransform(const glm::vec3& position, const float& rotation, const glm::vec2& size);
		static void SetQuadVertexBuffer(const glm::mat4& transfrom, const glm::vec4& color, const float& textureIndex, const float& tiling);
	};
	
}
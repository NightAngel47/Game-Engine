#pragma once
#include <Engine.h>

namespace Engine
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;
		
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		
		virtual void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& event) override;
	private:
		OrthographicCameraController m_CameraController;
		Ref<Framebuffer> m_Framebuffer;
		glm::vec2 m_ViewportSize = glm::vec2{1.0f};

		glm::vec4 m_ClearColor = {0.1f, 0.1f, 0.1f, 1.0f};
		glm::vec4 m_SquareColor = {0.8f, 0.2f, 0.3f, 1.0f};
		glm::vec4 m_SmallSquareColor = {0.2f, 0.3f, 0.8f, 1.0f};
		glm::vec4 m_RectColor = {0.3f, 0.8f, 0.2f, 1.0f};

		Ref<Texture2D> m_ShipTexture, m_TempleTexture, m_CheckerboardTexture, m_SpriteSheet;
		Ref<SubTexture2D> m_TextureStairs, m_TextureBarrel, m_TextureTree;

		float m_SmallSquareRotation = 0.0f;
		float m_RotationSpeed = 5.0f;

		uint32_t m_MapWidth, m_MapHeight;
		std::unordered_map<char, Engine::Ref<SubTexture2D>> s_TextureMap;
	};

}

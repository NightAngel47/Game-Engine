#pragma once
#include <Engine.h>

#include "ParticleSystem.h"

class Sandbox2D : public Engine::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;
	
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	
	virtual void OnUpdate(Engine::Timestep ts) override;
	virtual void OnImGuiRender() override;
	virtual void OnEvent(Engine::Event& event) override;
private:
	Engine::OrthographicCameraController m_CameraController;

	glm::vec4 m_ClearColor = {0.1f, 0.1f, 0.1f, 1.0f};
	glm::vec4 m_SquareColor = {0.8f, 0.2f, 0.3f, 1.0f};
	glm::vec4 m_SmallSquareColor = {0.2f, 0.3f, 0.8f, 1.0f};
	glm::vec4 m_RectColor = {0.3f, 0.8f, 0.2f, 1.0f};

	Engine::Ref<Engine::Texture2D> m_ShipTexture, m_TempleTexture, m_CheckerboardTexture, m_SpriteSheet;
	Engine::Ref<Engine::SubTexture2D> m_TextureStairs, m_TextureBarrel, m_TextureTree;

	float m_SmallSquareRotation = 0.0f;
	float m_RotationSpeed = 5.0f;

	ParticleProps m_Particle;
	ParticleSystem m_ParticleSystem;

	uint32_t m_MapWidth, m_MapHeight;
	std::unordered_map<char, Engine::Ref<Engine::SubTexture2D>> s_TextureMap;
};

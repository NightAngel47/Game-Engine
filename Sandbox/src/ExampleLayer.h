#pragma once
#include <Engine.h>

class ExampleLayer : public Engine::Layer
{
public:
	ExampleLayer();
	virtual ~ExampleLayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(Engine::Timestep ts) override;
	virtual void OnImGuiRender() override;
	virtual void OnEvent(Engine::Event& e) override;
private:
	Engine::ShaderLibrary m_ShaderLibrary;
	Engine::Ref<Engine::Shader> m_Shader;
	Engine::Ref<Engine::VertexArray> m_VertexArray;

	Engine::Ref<Engine::Shader> m_FlatColorShader;
	Engine::Ref<Engine::VertexArray> m_SquareVA;

	Engine::Ref<Engine::Texture2D> m_Texture;

	Engine::OrthographicCameraController m_CameraController;

	glm::vec3 m_ShipPosition;
	float m_ShipMoveSpeed = 1.0f;

	glm::vec4 m_SquareColor = {0.2f, 0.3f, 0.8f, 1.0f};
};
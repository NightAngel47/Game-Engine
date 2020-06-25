#pragma once
#include <Engine.h>

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

	// Temp
	Engine::Ref<Engine::VertexArray> m_VertexArray;
	Engine::Ref<Engine::Shader> m_FlatColorShader;
	
	glm::vec4 m_SquareColor = {0.8f, 0.2f, 0.3f, 1.0f};
};

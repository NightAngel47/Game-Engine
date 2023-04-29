#pragma once
#include <Engine.h>

class Standalone : public Engine::Layer
{
public:
	Standalone()
		: Engine::Layer("StandaloneLayer") {}
	virtual ~Standalone() = default;

	void OnAttach() override;
	void OnDetach() override;

	void OnUpdate(Engine::Timestep ts) override;
	void OnImGuiRender() override;
	void OnEvent(Engine::Event& e) override;

private:
	Engine::Ref<Engine::Scene> m_ActiveScene;
};

#include "Sandbox2D.h"

#include "imgui/imgui.h"

#include "glm/gtc/type_ptr.hpp"
#include "Platform/OpenGL/OpenGLShader.h"

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f, true)
{
}

void Sandbox2D::OnAttach()
{
	m_VertexArray = Engine::VertexArray::Create();
		
	float vertices[3 * 4] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.5f,  0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f
	};

	Engine::Ref<Engine::VertexBuffer> vertexBuffer = Engine::VertexBuffer::Create(vertices, sizeof(vertices));
	vertexBuffer->SetLayout({
		{ Engine::ShaderDataType::Float3, "a_Position" }
	});
	m_VertexArray->AddVertexBuffer(vertexBuffer);
	
	uint32_t indices[6] = { 0, 1, 2, 2, 3, 0 };
	Engine::Ref<Engine::IndexBuffer> indexBuffer = Engine::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
	m_VertexArray->SetIndexBuffer(indexBuffer);

	m_FlatColorShader = Engine::Shader::Create("assets/shaders/FlatColor.glsl");
}

void Sandbox2D::OnDetach()
{
	
}

void Sandbox2D::OnUpdate(Engine::Timestep ts)
{
	// Update
	m_CameraController.OnUpdate(ts);
	
	// Render
	Engine::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
	Engine::RenderCommand::Clear();
	
	Engine::Renderer::BeginScene(m_CameraController.GetCamera());

	std::dynamic_pointer_cast<Engine::OpenGLShader>(m_FlatColorShader)->Bind();
	std::dynamic_pointer_cast<Engine::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat4("u_Color", m_SquareColor);
	
	const glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
	
	for(int x = 0; x < 20; x++)
	{
		for(int y = 0; y < 20; y++)
		{
			glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
			glm::mat4 squareTransform = glm::translate(glm::mat4(1.0f), pos) * scale;
			Engine::Renderer::Submit(m_FlatColorShader, m_VertexArray, squareTransform);
		}
	}
	
	Engine::Renderer::EndScene();
}

void Sandbox2D::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void Sandbox2D::OnEvent(Engine::Event& e)
{
	m_CameraController.OnEvent(e);
}

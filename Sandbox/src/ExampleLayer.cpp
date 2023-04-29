#include "ExampleLayer.h"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

ExampleLayer::ExampleLayer()
	: Layer("Example"), m_CameraController(1280.0f / 720.0f, true), m_ShipPosition(0.0f)
{
}

void ExampleLayer::OnAttach()
{
	m_VertexArray = Engine::VertexArray::Create();

	float vertices[3 * 7] = {
		-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
		 0.5f, -0.5f, 0.0f,	0.2f, 0.3f, 0.8f, 1.0f,
		 0.0f,  0.5f, 0.0f,	0.8f, 0.8f, 0.2f, 1.0f
	};

	Engine::Ref<Engine::VertexBuffer> vertexBuffer = Engine::VertexBuffer::Create(vertices, sizeof(vertices));
	Engine::BufferLayout layout = {
		{ Engine::ShaderDataType::Float3, "a_Position" },
		{ Engine::ShaderDataType::Float4, "a_Color" }
	};
	
	vertexBuffer->SetLayout(layout);
	m_VertexArray->AddVertexBuffer(vertexBuffer);

	uint32_t indices[3] = { 0, 1, 2 };
	Engine::Ref<Engine::IndexBuffer> indexBuffer = Engine::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
	m_VertexArray->SetIndexBuffer(indexBuffer);

	m_SquareVA = Engine::VertexArray::Create();
	
	float squareVertices[5 * 4] = {
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 
		 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 
		 0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
	};

	Engine::Ref<Engine::VertexBuffer> squareVB = Engine::VertexBuffer::Create(squareVertices, sizeof(squareVertices));
	squareVB->SetLayout({
		{ Engine::ShaderDataType::Float3, "a_Position" },
		{ Engine::ShaderDataType::Float2, "a_TexCoord" }
	});
	m_SquareVA->AddVertexBuffer(squareVB);
	
	uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
	Engine::Ref<Engine::IndexBuffer> squareIB = Engine::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
	m_SquareVA->SetIndexBuffer(squareIB);
	
	std::string vertexSrc = R"(
		#version 330 core

		layout(location = 0) in vec3 a_Position;
		layout(location = 1) in vec4 a_Color;

		uniform mat4 u_ViewProjection;
		uniform mat4 u_Transform;

		out vec3 v_Position;
		out vec4 v_Color;
	
		void main()
		{
			v_Position = a_Position;
			v_Color = a_Color;
			gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
		}
	)";
	
	std::string fragmentSrc = R"(
		#version 330 core

		layout(location = 0) out vec4 color;

		in vec3 v_Position;
		in vec4 v_Color;
	
		void main()
		{
			color = vec4(v_Position * 0.5 + 0.5, 1.0);
			color = v_Color;
		}
	)";
	
	m_Shader = Engine::Shader::Create("VertexPosColor", vertexSrc, fragmentSrc);
	
	m_FlatColorShader = Engine::Shader::Create("assets/shaders/FlatColor.glsl");

	auto textureShader = m_ShaderLibrary.Load("assets/shaders/Renderer2D_Quad.glsl");

	m_Texture = Engine::Texture2D::Create("assets/textures/shipGreen_manned.png");
	
	textureShader->Bind();
	textureShader->SetInt("u_Texture", 0);
}

void ExampleLayer::OnDetach()
{
}

void ExampleLayer::OnUpdate(Engine::Timestep ts)
{
	// Update
	m_CameraController.OnUpdate(ts);
	
	if(Engine::Input::IsKeyPressed(Engine::Key::Left))
		m_ShipPosition.x -= m_ShipMoveSpeed * ts;
	if(Engine::Input::IsKeyPressed(Engine::Key::Right))
		m_ShipPosition.x += m_ShipMoveSpeed * ts;
	
	if(Engine::Input::IsKeyPressed(Engine::Key::Up))
		m_ShipPosition.y += m_ShipMoveSpeed * ts;
	if(Engine::Input::IsKeyPressed(Engine::Key::Down))
		m_ShipPosition.y -= m_ShipMoveSpeed * ts;

	// Render
	Engine::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
	Engine::RenderCommand::Clear();
	
	Engine::Renderer2D::BeginScene(m_CameraController.GetCamera(), glm::mat4(1.0f));

	const glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

	m_FlatColorShader->Bind();
	m_FlatColorShader->SetFloat4("u_Color", m_SquareColor);
	
	for(int x = 0; x < 20; x++)
	{
		for(int y = 0; y < 20; y++)
		{
			glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
			glm::mat4 squareTransform = glm::translate(glm::mat4(1.0f), pos) * scale;
			Engine::Renderer::Submit(m_FlatColorShader, m_SquareVA, squareTransform);
		}
	}

	auto textureShader = m_ShaderLibrary.Get("Texture");
	
	m_Texture->Bind();
	const glm::mat4 shipScale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
	const glm::mat4 shipTransform = glm::translate(glm::mat4(1.0f), m_ShipPosition) * shipScale;
	Engine::Renderer::Submit(textureShader, m_SquareVA, shipTransform);

	// Triangle
	// Engine::Renderer::Submit(m_Shader, m_VertexArray);
	
	Engine::Renderer2D::EndScene();
}

void ExampleLayer::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void ExampleLayer::OnEvent(Engine::Event& e)
{
	m_CameraController.OnEvent(e);
}

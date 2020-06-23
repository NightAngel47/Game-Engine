#include <Engine.h>

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>

class ExampleLayer : public Engine::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f), m_TrianglePosition(0.0f)
	{
		m_VertexArray.reset(Engine::VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f,	0.2f, 0.3f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f,	0.8f, 0.8f, 0.2f, 1.0f
		};
		
		std::shared_ptr<Engine::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Engine::VertexBuffer::Create(vertices, sizeof(vertices)));
		Engine::BufferLayout layout = {
			{ Engine::ShaderDataType::Float3, "a_Position" },
			{ Engine::ShaderDataType::Float4, "a_Color" }
		};
		
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[3] = { 0, 1, 2 };
		std::shared_ptr<Engine::IndexBuffer> indexBuffer;
		indexBuffer.reset(Engine::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_SquareVA.reset(Engine::VertexArray::Create());
		
		float squareVertices[3 * 4] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.5f,  0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f
		};
		
		std::shared_ptr<Engine::VertexBuffer> squareVB;
		squareVB.reset(Engine::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout({
			{ Engine::ShaderDataType::Float3, "a_Position" }
		});
		m_SquareVA->AddVertexBuffer(squareVB);
		
		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<Engine::IndexBuffer> squareIB;
		squareIB.reset(Engine::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
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
		
		m_Shader.reset(new Engine::Shader(vertexSrc, fragmentSrc));

		std::string squareShaderVertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
		
			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
		
			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";
		
		std::string squareShaderFragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;
		
			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color += vec4(0.25, 0.25, 0.25, 0.0);
			}
		)";
		
		m_SquareShader.reset(new Engine::Shader(squareShaderVertexSrc, squareShaderFragmentSrc));
	}

	void OnUpdate(Engine::Timestep ts) override
	{
		ENGINE_TRACE("Delta time: {0}s ({1}ms)", ts, ts.GetMilliseconds());

		if(Engine::Input::IsKeyPressed(ENGINE_KEY_D) || Engine::Input::IsKeyPressed(ENGINE_KEY_RIGHT))
			m_CameraPosition.x -= m_CameraMoveSpeed * ts;
		if(Engine::Input::IsKeyPressed(ENGINE_KEY_A) || Engine::Input::IsKeyPressed(ENGINE_KEY_LEFT))
			m_CameraPosition.x += m_CameraMoveSpeed * ts;
		
		if(Engine::Input::IsKeyPressed(ENGINE_KEY_W) || Engine::Input::IsKeyPressed(ENGINE_KEY_UP))
			m_CameraPosition.y -= m_CameraMoveSpeed * ts;
		if(Engine::Input::IsKeyPressed(ENGINE_KEY_S) || Engine::Input::IsKeyPressed(ENGINE_KEY_DOWN))
			m_CameraPosition.y += m_CameraMoveSpeed * ts;
		
		if(Engine::Input::IsKeyPressed(ENGINE_KEY_Q))
			m_CameraRotation -= m_CameraRotateSpeed * ts;
		if(Engine::Input::IsKeyPressed(ENGINE_KEY_E))
			m_CameraRotation += m_CameraRotateSpeed * ts;
		
		if(Engine::Input::IsKeyPressed(ENGINE_KEY_J))
			m_TrianglePosition.x -= m_TriangleMoveSpeed * ts;
		if(Engine::Input::IsKeyPressed(ENGINE_KEY_L))
			m_TrianglePosition.x += m_TriangleMoveSpeed * ts;
		
		if(Engine::Input::IsKeyPressed(ENGINE_KEY_I))
			m_TrianglePosition.y += m_TriangleMoveSpeed * ts;
		if(Engine::Input::IsKeyPressed(ENGINE_KEY_K))
			m_TrianglePosition.y -= m_TriangleMoveSpeed * ts;
		
		Engine::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
		Engine::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);
		
		Engine::Renderer::BeginScene(m_Camera);

		const glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		for(int x = 0; x < 20; x++)
		{
			for(int y = 0; y < 20; y++)
			{
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
				glm::mat4 squareTransform = glm::translate(glm::mat4(1.0f), pos) * scale;
				Engine::Renderer::Submit(m_SquareShader, m_SquareVA, squareTransform);
			}
		}

		glm::mat4 traingleTransfrom = glm::translate(glm::mat4(1.0f), m_TrianglePosition);
		Engine::Renderer::Submit(m_Shader, m_VertexArray, traingleTransfrom);
		
		Engine::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
		
	}

	void OnEvent(Engine::Event& event) override
	{
	}
	
private:
	std::shared_ptr<Engine::Shader> m_Shader;
	std::shared_ptr<Engine::VertexArray> m_VertexArray;
	
	std::shared_ptr<Engine::Shader> m_SquareShader;
	std::shared_ptr<Engine::VertexArray> m_SquareVA;

	Engine::OrthographicCamera m_Camera;

	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 2.5f;
	
	float m_CameraRotation = 0.0f;
	float m_CameraRotateSpeed = 90.0f;

	glm::vec3 m_TrianglePosition;
	float m_TriangleMoveSpeed = 1.0f;
};

class Sandbox : public Engine::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox()
	{
		
	}
	
};

Engine::Application* Engine::CreateApplication()
{
	return new Sandbox();
}
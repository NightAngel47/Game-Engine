#include <Engine.h>

#include "Platform/OpenGL/OpenGLShader.h"

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class ExampleLayer : public Engine::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f), m_ShipPosition(0.0f)
	{
		m_VertexArray.reset(Engine::VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f,	0.2f, 0.3f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f,	0.8f, 0.8f, 0.2f, 1.0f
		};
		
		Engine::Ref<Engine::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Engine::VertexBuffer::Create(vertices, sizeof(vertices)));
		Engine::BufferLayout layout = {
			{ Engine::ShaderDataType::Float3, "a_Position" },
			{ Engine::ShaderDataType::Float4, "a_Color" }
		};
		
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[3] = { 0, 1, 2 };
		Engine::Ref<Engine::IndexBuffer> indexBuffer;
		indexBuffer.reset(Engine::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_SquareVA.reset(Engine::VertexArray::Create());
		
		float squareVertices[5 * 4] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};
		
		Engine::Ref<Engine::VertexBuffer> squareVB;
		squareVB.reset(Engine::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout({
			{ Engine::ShaderDataType::Float3, "a_Position" },
			{ Engine::ShaderDataType::Float2, "a_TexCoord" }
		});
		m_SquareVA->AddVertexBuffer(squareVB);
		
		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		Engine::Ref<Engine::IndexBuffer> squareIB;
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
		
		m_Shader = Engine::Shader::Create("VertexPosColor", vertexSrc, fragmentSrc);

		std::string flatColorShaderVertexSrc = R"(
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
		
		std::string flatColorShaderFragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			uniform vec3 u_Color;
		
			void main()
			{
				color = vec4(u_Color, 1.0);
			}
		)";
		
		m_FlatColorShader = Engine::Shader::Create("FlatColor", flatColorShaderVertexSrc, flatColorShaderFragmentSrc);

		auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

		m_Texture = Engine::Texture2D::Create("assets/textures/shipGreen_manned.png");
		
		std::dynamic_pointer_cast<Engine::OpenGLShader>(textureShader)->Bind();
		std::dynamic_pointer_cast<Engine::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);
	}

	void OnUpdate(Engine::Timestep ts) override
	{
		// ENGINE_TRACE("Delta time: {0}s ({1}ms)", ts, ts.GetMilliseconds());

		if(Engine::Input::IsKeyPressed(ENGINE_KEY_RIGHT))
			m_CameraPosition.x += m_CameraMoveSpeed * ts;
		if(Engine::Input::IsKeyPressed(ENGINE_KEY_LEFT))
			m_CameraPosition.x -= m_CameraMoveSpeed * ts;
		
		if(Engine::Input::IsKeyPressed(ENGINE_KEY_UP))
			m_CameraPosition.y += m_CameraMoveSpeed * ts;
		if(Engine::Input::IsKeyPressed(ENGINE_KEY_DOWN))
			m_CameraPosition.y -= m_CameraMoveSpeed * ts;
		
		if(Engine::Input::IsKeyPressed(ENGINE_KEY_Q))
			m_CameraRotation -= m_CameraRotateSpeed * ts;
		if(Engine::Input::IsKeyPressed(ENGINE_KEY_E))
			m_CameraRotation += m_CameraRotateSpeed * ts;
		
		if(Engine::Input::IsKeyPressed(ENGINE_KEY_A))
			m_ShipPosition.x -= m_ShipMoveSpeed * ts;
		if(Engine::Input::IsKeyPressed(ENGINE_KEY_D))
			m_ShipPosition.x += m_ShipMoveSpeed * ts;
		
		if(Engine::Input::IsKeyPressed(ENGINE_KEY_W))
			m_ShipPosition.y += m_ShipMoveSpeed * ts;
		if(Engine::Input::IsKeyPressed(ENGINE_KEY_S))
			m_ShipPosition.y -= m_ShipMoveSpeed * ts;
		
		Engine::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
		Engine::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);
		
		Engine::Renderer::BeginScene(m_Camera);

		const glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		std::dynamic_pointer_cast<Engine::OpenGLShader>(m_FlatColorShader)->Bind();
		std::dynamic_pointer_cast<Engine::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);
		
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
		
		Engine::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::End();
	}

	void OnEvent(Engine::Event& event) override
	{
	}
	
private:
	Engine::ShaderLibrary m_ShaderLibrary;
	Engine::Ref<Engine::Shader> m_Shader;
	Engine::Ref<Engine::VertexArray> m_VertexArray;
	
	Engine::Ref<Engine::Shader> m_FlatColorShader;
	Engine::Ref<Engine::VertexArray> m_SquareVA;

	Engine::Ref<Engine::Texture2D> m_Texture;

	Engine::OrthographicCamera m_Camera;

	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 2.5f;
	
	float m_CameraRotation = 0.0f;
	float m_CameraRotateSpeed = 90.0f;

	glm::vec3 m_ShipPosition;
	float m_ShipMoveSpeed = 1.0f;

	glm::vec3 m_SquareColor = {0.2f, 0.3f, 0.8f};
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
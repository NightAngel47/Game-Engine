#include "enginepch.h"
#include "Application.h"

#include "Engine/Log.h"

#include "Renderer/Renderer.h"

#include "Input.h"
#include "KeyCodes.h"
#include "MouseButtonCodes.h"

namespace Engine
{
	Application* Application::s_Instance = nullptr;
	
	Application::Application()
		:m_Camera(-1.6f, 1.6f, -0.9f, 0.9f)
	{
		ENGINE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;
		
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallBack(ENGINE_BIND_EVENT_FN(Application::OnEvent));

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		m_VertexArray.reset(VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f,	0.2f, 0.3f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f,	0.8f, 0.8f, 0.2f, 1.0f
		};
		
		std::shared_ptr<VertexBuffer> vertexBuffer;
		vertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));
		BufferLayout layout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" }
		};
		
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[3] = { 0, 1, 2 };
		std::shared_ptr<IndexBuffer> indexBuffer;
		indexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_SquareVA.reset(VertexArray::Create());
		
		float squareVertices[3 * 4] = {
			-0.75f, -0.75f, 0.0f,
			 0.75f, -0.75f, 0.0f,
			 0.75f,  0.75f, 0.0f,
			-0.75f,  0.75f, 0.0f
		};
		
		std::shared_ptr<VertexBuffer> squareVB;
		squareVB.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout({
			{ ShaderDataType::Float3, "a_Position" }
		});
		m_SquareVA->AddVertexBuffer(squareVB);
		
		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<IndexBuffer> squareIB;
		squareIB.reset(IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);
		
		std::string vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;

			out vec3 v_Position;
			out vec4 v_Color;
		
			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
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
		
		m_Shader.reset(new Shader(vertexSrc, fragmentSrc));

		std::string squareShaderVertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
		
			uniform mat4 u_ViewProjection;

			out vec3 v_Position;
		
			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
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
		
		m_SquareShader.reset(new Shader(squareShaderVertexSrc, squareShaderFragmentSrc));
	}

	Application::~Application()
	{
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}
	
	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(ENGINE_BIND_EVENT_FN(Application::OnWindowClose));

		if(Engine::Input::IsKeyPressed(ENGINE_KEY_W) || Engine::Input::IsKeyPressed(ENGINE_KEY_UP))
			m_VerticalMovement -= m_MoveSpeed;
		if(Engine::Input::IsKeyPressed(ENGINE_KEY_A) || Engine::Input::IsKeyPressed(ENGINE_KEY_LEFT))
			m_HorizontalMovement += m_MoveSpeed;
		if(Engine::Input::IsKeyPressed(ENGINE_KEY_S) || Engine::Input::IsKeyPressed(ENGINE_KEY_DOWN))
			m_VerticalMovement += m_MoveSpeed;
		if(Engine::Input::IsKeyPressed(ENGINE_KEY_D) || Engine::Input::IsKeyPressed(ENGINE_KEY_RIGHT))
			m_HorizontalMovement -= m_MoveSpeed;
		if(Engine::Input::IsKeyPressed(ENGINE_KEY_Q) || Engine::Input::IsMouseButtonPressed(ENGINE_MOUSE_BUTTON_1))
			m_RotationMovement -= m_RotateSpeed;
		if(Engine::Input::IsKeyPressed(ENGINE_KEY_E) || Engine::Input::IsMouseButtonPressed(ENGINE_MOUSE_BUTTON_2))
			m_RotationMovement += m_RotateSpeed;

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	void Application::Run()
	{
		while (m_Running)
		{
			RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
			RenderCommand::Clear();

			m_Camera.SetPosition({m_HorizontalMovement, m_VerticalMovement, 0.0f});
			m_Camera.SetRotation(m_RotationMovement);
			
			Renderer::BeginScene(m_Camera);
			
			Renderer::Submit(m_SquareShader, m_SquareVA);
			Renderer::Submit(m_Shader, m_VertexArray);
			
			Renderer::EndScene();

			for (Layer* layer : m_LayerStack)
				layer->OnUpdate();

			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();
			m_ImGuiLayer->End();
			
			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
}

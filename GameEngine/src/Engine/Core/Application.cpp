#include "enginepch.h"
#include "Engine/Core/Application.h"

#include "Engine/Renderer/Renderer.h"
#include "Engine/Scripting/CsBind.h"

#include <GLFW/glfw3.h>

#include <mono/metadata/mono-config.h>

namespace Engine
{
	Application* Application::s_Instance = nullptr;
	
	Application::Application(const std::string& name, ApplicationCommandLineArgs args)
		:m_CommandLineArgs(args)
	{
		ENGINE_PROFILE_FUNCTION();
		
		ENGINE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;
		
		m_Window = Scope<Window>(Window::Create(WindowProps(name)));
		m_Window->SetEventCallBack(ENGINE_BIND_EVENT_FN(Application::OnEvent));

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);


		// Mono
		// TODO move Mono to vendor directory and copy necessary items to bin dir
		mono_set_dirs(R"(..\bin\Debug-windows-x86_64\Engine-Editor\Mono\lib)", R"(..\bin\Debug-windows-x86_64\Engine-Editor\Mono\etc)"); 

		m_MonoDomain = mono_jit_init("GameEngineScripting");
		if (!m_MonoDomain)
		{
			ENGINE_CORE_CRITICAL("Mono Domain could not be initialized!");
		}
		else
		{
			ENGINE_CORE_TRACE("Mono Doman initialized!");
		}
		
		MonoImageOpenStatus status = MONO_IMAGE_OK;
		m_MonoAssembly = mono_assembly_open(R"(..\bin\Debug-windows-x86_64\GameEngineScripting\GameEngineScripting.dll)", &status);
		switch (status)
		{
		case MONO_IMAGE_OK:
			ENGINE_CORE_TRACE("Mono Assembly opened!");
			break;
		case MONO_IMAGE_ERROR_ERRNO:
			ENGINE_CORE_CRITICAL("Mono Assembly: MONO_IMAGE_ERROR_ERRNO!");
			break;
		case MONO_IMAGE_MISSING_ASSEMBLYREF:
			ENGINE_CORE_CRITICAL("Mono Assembly: MONO_IMAGE_MISSING_ASSEMBLYREF!");
			break;
		case MONO_IMAGE_IMAGE_INVALID:
			ENGINE_CORE_CRITICAL("Mono Assembly: MONO_IMAGE_IMAGE_INVALID!");
			break;
		}

		m_MonoAssemblyImage = mono_assembly_get_image(m_MonoAssembly);
		if (!m_MonoAssemblyImage)
		{
			ENGINE_CORE_CRITICAL("Mono Image could not be set!");
		}
		else
		{
			ENGINE_CORE_TRACE("Mono Image set!");
		}

		// Add internal calls
		mono_add_internal_call("GEL.Log::Trace(string)", &CsBind::CS_Log_Trace);

		// Find IGame
		MonoClass* ptrIGameClass = mono_class_from_name(m_MonoAssemblyImage, "GES", "IGame");
		MonoClass* ptrMainClass = mono_class_from_name(m_MonoAssemblyImage, "GES", "GameMain");
		if (ptrIGameClass && ptrMainClass)
		{
			MonoMethodDesc* ptrMainMethodDesc = mono_method_desc_new(".GameMain:main()", false);
			if (ptrMainMethodDesc)
			{
				MonoMethod* ptrMainMethod = mono_method_desc_search_in_class(ptrMainMethodDesc, ptrMainClass);
				if (ptrMainMethod)
				{
					// Call main method
					MonoObject* ptrExObject = nullptr;
					m_PtrGameObject = mono_runtime_invoke(ptrMainMethod, nullptr, nullptr, &ptrExObject);
					if (m_PtrGameObject)
					{
						// Ref count on c++ side
						m_GameObjectGCHandle = mono_gchandle_new(m_PtrGameObject, false);

						// Get tick function
						MonoMethodDesc* ptrTickMethodDesc = mono_method_desc_new(".IGame:tick()", false);
						if (ptrTickMethodDesc)
						{
							// Get real function
							MonoMethod* virtualMethod = mono_method_desc_search_in_class(ptrTickMethodDesc, ptrIGameClass);
							if (virtualMethod)
							{
								m_PtrTickMethod = mono_object_get_virtual_method(m_PtrGameObject, virtualMethod);
							}

							// Free
							mono_method_desc_free(ptrTickMethodDesc);
						}
					}

					// Report Exception
					if (ptrExObject)
					{
						MonoString* exString = mono_object_to_string(ptrExObject, nullptr);
						const char* exCString = mono_string_to_utf8(exString);
						ENGINE_CORE_ERROR(exCString);
					}
				}

				// Free desc
				mono_method_desc_free(ptrMainMethodDesc);
			}
		}
	}

	Application::~Application()
	{
		ENGINE_PROFILE_FUNCTION();
		
		Renderer::Shutdown();

		// Mono
		// Release mono handles
		if (m_GameObjectGCHandle)
		{
			mono_gchandle_free(m_GameObjectGCHandle);
		}

		// Release Domain
		if (m_MonoDomain)
		{
			mono_jit_cleanup(m_MonoDomain);
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		ENGINE_PROFILE_FUNCTION();
		
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		ENGINE_PROFILE_FUNCTION();
		
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::Close()
	{
		m_Running = false;
	}

	void Application::OnEvent(Event& e)
	{
		ENGINE_PROFILE_FUNCTION();
		
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(ENGINE_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(ENGINE_BIND_EVENT_FN(Application::OnWindowResize));

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			(*it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	void Application::Run()
	{	
		ENGINE_PROFILE_FUNCTION();
		
		while (m_Running)
		{
			ENGINE_PROFILE_SCOPE("RunLoop");
		
			float time = (float)glfwGetTime(); // Platform::GetTime
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			// Run Mono Tick function
			if (m_PtrTickMethod)
			{
				MonoObject* ptrExObject = nullptr;
				mono_runtime_invoke(m_PtrTickMethod, m_PtrGameObject, nullptr, &ptrExObject);

				// Report Exception
				if (ptrExObject)
				{
					MonoString* exString = mono_object_to_string(ptrExObject, nullptr);
					const char* exCString = mono_string_to_utf8(exString);
					ENGINE_CORE_ERROR(exCString);
				}
			}

			if (!m_Minimized)
			{
				{
					ENGINE_PROFILE_SCOPE("LayerStack OnUpdate");
					
					for (Layer* layer : m_LayerStack)
						layer->OnUpdate(timestep);
				}
			}

			m_ImGuiLayer->Begin();
			{
				ENGINE_PROFILE_SCOPE("LayerStack OnImGuiRender");
				
				for (Layer* layer : m_LayerStack)
					layer->OnImGuiRender();
			}
			m_ImGuiLayer->End();
			
			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		ENGINE_PROFILE_FUNCTION();
		
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		ENGINE_PROFILE_FUNCTION();
		
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}
		
		m_Minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
		
		return false;
	}
}

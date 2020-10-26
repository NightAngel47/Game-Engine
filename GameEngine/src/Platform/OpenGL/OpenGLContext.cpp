#include "enginepch.h"
#include "Platform/OpenGL/OpenGLContext.h"

#include <glad/glad.h>

namespace Engine
{
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		ENGINE_CORE_ASSERT(windowHandle, "Window handle is null!");
	}

	void OpenGLContext::Init()
	{
		ENGINE_PROFILE_FUNCTION();
		
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		ENGINE_CORE_ASSERT(status, "Failed to initalize Glad!");

		ENGINE_CORE_INFO("OpenGL Info:");
		ENGINE_CORE_INFO("	Vendor:		{0}", glGetString(GL_VENDOR));
		ENGINE_CORE_INFO("	Renderer:	{0}", glGetString(GL_RENDERER));
		ENGINE_CORE_INFO("	Version:	{0}", glGetString(GL_VERSION));

		ENGINE_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5), "Engine requires at least OpenGL version 4.5!");
	}

	void OpenGLContext::SwapBuffers()
	{
		ENGINE_PROFILE_FUNCTION();
		
		glfwSwapBuffers(m_WindowHandle);
	}
}

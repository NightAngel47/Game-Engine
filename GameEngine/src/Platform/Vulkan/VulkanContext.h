#pragma once
#include "Engine/Renderer/GraphicsContext.h"

#include <GLFW/glfw3.h>

namespace Engine
{
	class VulkanContext : public GraphicsContext
	{
	public:
		VulkanContext(GLFWwindow* windowHandle);
		
		virtual void Init() override;
		virtual void Shutdown() override;
		virtual void SwapBuffers() override;

	private:
		void CreateInstance();
		void SetupDebugMessenger();
		void CreateSurface();
		void PickPhysicalDevice();
		void CreateLogicalDevice();
		void CreateSwapChain();
	private:
		GLFWwindow* m_WindowHandle;

		VkInstance m_Instance;
		VkDebugUtilsMessengerEXT m_DebugMessenger;
		
		VkSurfaceKHR m_Surface;

		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		VkDevice m_Device;

		VkQueue m_GraphicsQueue;
		VkQueue m_PresentQueue;

		VkSurfaceFormatKHR m_SurfaceFormat;
		VkPresentModeKHR m_PresentMode;
		VkExtent2D m_Extent;
		VkSwapchainKHR m_SwapChain;
		std::vector<VkImage> m_SwapChainImages;
	};
}

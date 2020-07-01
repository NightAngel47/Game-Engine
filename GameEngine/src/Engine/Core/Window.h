#pragma once
#include "enginepch.h"

#include "Engine/Core/Base.h"
#include "Engine/Events/Event.h"

namespace Engine
{
	struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		WindowProps(const std::string& title = "Game Engine", 
					uint32_t width = 1280, 
					uint32_t height = 720)
			: Title(title), Width(width), Height(height)
		{	
		}
	};

	// Interface representing a desktop system based Window
	class Window
	{
	public:
		using EventCallBackFn = std::function<void(Event&)>;

		virtual ~Window() = default;

		virtual void OnUpdate() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		// Window attributes
		virtual void SetEventCallBack(const EventCallBackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;

		static Scope<Window> Create(const WindowProps& = WindowProps());
	};
}

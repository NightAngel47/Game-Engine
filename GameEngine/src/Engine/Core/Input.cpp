#include "enginepch.h"
#include "Engine/Core/Input.h"

#ifdef ENGINE_PLATFORM_WINDOWS
	#include "Platform/Windows/WindowsInput.h"
#endif

namespace Engine {

	Scope<Input> Input::s_Instance = Input::Create();

	Scope<Input> Input::Create()
	{
		#ifdef ENGINE_PLATFORM_WINDOWS
			return CreateScope<WindowsInput>();
		#else
			ENGINE_CORE_ASSERT(false, "Unknown platform!");
			return nullptr;
		#endif
	}
	
}  
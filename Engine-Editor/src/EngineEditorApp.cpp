#include <Engine.h>
#include <Engine/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Engine
{
	
	class EngineEditor : public Application
	{
	public:
		EngineEditor()
			: Application("Engine Editor")
		{
			ENGINE_PROFILE_FUNCTION();
			
			PushLayer(new EditorLayer());
		}

		~EngineEditor()
		{
			ENGINE_PROFILE_FUNCTION();
		}
	};

	Application* CreateApplication()
	{
		return new EngineEditor();
	}
	
}
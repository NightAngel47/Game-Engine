#include <Engine.h>
#include <Engine/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Engine
{
	
	class EngineEditor : public Application
	{
	public:
		EngineEditor(ApplicationCommandLineArgs args)
			: Application("Engine Editor", args)
		{
			ENGINE_PROFILE_FUNCTION();
			
			PushLayer(new EditorLayer());
		}

		~EngineEditor()
		{
			ENGINE_PROFILE_FUNCTION();
		}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		return new EngineEditor(args);
	}
	
}

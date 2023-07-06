#include <Engine.h>
#include <Engine/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Engine
{
	
	class EngineEditor : public Application
	{
	public:
		EngineEditor(const ApplicationSpecification& specification)
			: Application(specification)
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
		ApplicationSpecification spec;
		spec.Name = "Editor";
		spec.CommandLineArgs = args;
		spec.Runtime = false;

		return new EngineEditor(spec);
	}
	
}

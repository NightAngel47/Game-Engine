project "Engine-ScriptCore"
	kind "SharedLib"
	language "C#"
	dotnetframework "4.8"
	csversion "10.0"

	targetdir ("../Engine-Editor/Resources/Scripts/Binaries")
	objdir ("../Engine-Editor/Resources/Scripts/Intermediates")

	files
	{
		"Source/**.cs",
		"Properties/**.cs",
	}
	
	filter "configurations:Debug"
		defines "ENGINE_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "ENGINE_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "ENGINE_DIST"
		runtime "Release"
		optimize "on"

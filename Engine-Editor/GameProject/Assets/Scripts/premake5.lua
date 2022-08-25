local EngineRootDir = '../../../..'

include (EngineRootDir .. "/vendor/premake/premake_customization/solution_items.lua")

workspace "GameProject"
	architecture "x86_64"
	startproject "GameProject"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "GameProject"
	kind "SharedLib"
	language "C#"
	dotnetframework "4.8"
	csversion "10.0"

	targetdir ("Binaries")
	objdir ("Intermediates")

	files
	{
		"Source/**.cs",
		"Properties/**.cs",
	}

	links
	{
		"Engine-ScriptCore"
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

group "Engine"

include (EngineRootDir .. "/Engine-ScriptCore")

group ""
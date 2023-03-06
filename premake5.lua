include "./vendor/premake/premake_customization/solution_items.lua"
include "Dependencies.lua"

workspace "GameEngine"
	architecture "x86_64"
	startproject "Engine-Editor"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	solution_items
	{
		".editorconfig"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "vendor/premake"
	include "GameEngine/vendor/GLFW"
	include "GameEngine/vendor/Glad"
	include "GameEngine/vendor/imgui"
	include "GameEngine/vendor/yaml-cpp"
	include "GameEngine/vendor/box2d"
	include "GameEngine/vendor/msdf-atlas-gen"
group ""

group "Core"
	include "GameEngine"
	include "Engine-ScriptCore"
group ""

group "Tools"
	include "Engine-Editor"
group ""

group "Misc"
	include "Sandbox"
group ""

project "Engine-Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"%{wks.location}/GameEngine/vendor/spdlog/include",
		"%{wks.location}/GameEngine/src",
		"%{wks.location}/GameEngine/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.Mono}"
	}

	links
	{
		"GameEngine"
	}

	postbuildcommands
	{
		"{COPY} %{LibraryDir.VulkanSDK_DLL} %{wks.location}/bin/" .. outputdir .. "/%{prj.name}",
		"{COPY} %{LibraryDir.Mono_Lib}/mono/ %{wks.location}/bin/" .. outputdir .. "/%{prj.name}/Mono/lib/mono",
		"{COPY} %{LibraryDir.Mono_Bin}/ %{wks.location}/bin/" .. outputdir .. "/%{prj.name}/Mono/bin",
		"{COPY} %{LibraryDir.Mono_Etc}/ %{wks.location}/bin/" .. outputdir .. "/%{prj.name}/Mono/etc",
		"{COPY} %{LibraryDir.Mono_DLL} %{wks.location}/bin/" .. outputdir .. "/%{prj.name}"
	}	

	filter "system:windows"
		systemversion "latest"

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

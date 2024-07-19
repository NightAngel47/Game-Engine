
-- Engine Dependencies

VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["stb_image"] = "%{wks.location}/GameEngine/vendor/stb_image"
IncludeDir["yaml_cpp"] = "%{wks.location}/GameEngine/vendor/yaml-cpp/include"
IncludeDir["GLFW"] = "%{wks.location}/GameEngine/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/GameEngine/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/GameEngine/vendor/imgui"
IncludeDir["ImGuizmo"] = "%{wks.location}/GameEngine/vendor/ImGuizmo"
IncludeDir["glm"] = "%{wks.location}/GameEngine/vendor/glm"
IncludeDir["entt"] = "%{wks.location}/GameEngine/vendor/entt/include"
IncludeDir["mono"] = "%{wks.location}/GameEngine/vendor/mono/include"
IncludeDir["box2d"] = "%{wks.location}/GameEngine/vendor/box2d/include"
IncludeDir["filewatch"] = "%{wks.location}/GameEngine/vendor/filewatch"
IncludeDir["miniaudio"] = "%{wks.location}/GameEngine/vendor/miniaudio"
IncludeDir["msdfgen"] = "%{wks.location}/GameEngine/vendor/msdf-atlas-gen/msdfgen"
IncludeDir["msdf_atlas_gen"] = "%{wks.location}/GameEngine/vendor/msdf-atlas-gen/msdf-atlas-gen"
IncludeDir["shaderc"] = "%{wks.location}/GameEngine/vendor/shaderc/include"
IncludeDir["SPIRV_Cross"] = "%{wks.location}/GameEngine/vendor/SPIRV-Cross/include"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"

LibraryDir = {}

LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"

LibraryDir["mono"] = "%{wks.location}/GameEngine/vendor/mono/lib/"

Library = {}
Library["mono"] = "%{LibraryDir.mono}/%{cfg.buildcfg}/libmono-static-sgen.lib"

Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{wks.location}/GameEngine/vendor/SPIRV-Cross/lib/Debug/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{wks.location}/GameEngine/vendor/SPIRV-Cross/lib/Debug/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{wks.location}/GameEngine/vendor/SPIRV-Cross/lib/Release/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{wks.location}/GameEngine/vendor/SPIRV-Cross/lib/Release/spirv-cross-glsl.lib"

-- Windows
Library["WinSock"] = "Ws2_32.lib"
Library["WinMM"] = "Winmm.lib"
Library["WinVersion"] = "Version.lib"
Library["BCrypt"] = "Bcrypt.lib"
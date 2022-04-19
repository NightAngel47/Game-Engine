
-- Engine Dependencies

VULKAN_SDK = os.getenv("VULKAN_SDK")
MONO = os.getenv("MONO")

IncludeDir = {}
IncludeDir["stb_image"] = "%{wks.location}/GameEngine/vendor/stb_image"
IncludeDir["yaml_cpp"] = "%{wks.location}/GameEngine/vendor/yaml-cpp/include"
IncludeDir["GLFW"] = "%{wks.location}/GameEngine/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/GameEngine/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/GameEngine/vendor/imgui"
IncludeDir["ImGuizmo"] = "%{wks.location}/GameEngine/vendor/ImGuizmo"
IncludeDir["glm"] = "%{wks.location}/GameEngine/vendor/glm"
IncludeDir["entt"] = "%{wks.location}/GameEngine/vendor/entt/include"
IncludeDir["box2d"] = "%{wks.location}/GameEngine/vendor/box2d/include"
IncludeDir["shaderc"] = "%{wks.location}/GameEngine/vendor/shaderc/include"
IncludeDir["SPIRV_Cross"] = "%{wks.location}/GameEngine/vendor/SPIRV-Cross"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"
IncludeDir["Mono"] = "%{MONO}/include/mono-2.0"

LibraryDir = {}

LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
LibraryDir["VulkanSDK_Debug"] = "%{wks.location}/GameEngine/vendor/VulkanSDK/Lib"
LibraryDir["VulkanSDK_DLL"] = "%{wks.location}/GameEngine/vendor/VulkanSDK/Bin"

LibraryDir["Mono_Lib"] = "%{wks.location}/GameEngine/vendor/Mono/lib"
LibraryDir["Mono_Bin"] = "%{wks.location}/GameEngine/vendor/Mono/bin"
LibraryDir["Mono_Etc"] = "%{wks.location}/GameEngine/vendor/Mono/etc"
LibraryDir["Mono_DLL"] = "%{wks.location}/GameEngine/vendor/Mono/bin/mono-2.0-sgen.dll"

Library = {}
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"

Library["Mono_Lib_sgen"] = "%{LibraryDir.Mono_Lib}/mono-2.0-sgen.lib"
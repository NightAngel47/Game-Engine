#include "enginepch.h"
#include "Platform/Vulkan/VulkanShader.h"

#include <filesystem>
#include <fstream>

#include <glm/gtc/type_ptr.hpp>

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

#include "Engine/Core/Timer.h"

namespace Engine
{
	namespace Utils
	{
		static const char* GetCacheDirectory()
		{
			// TODO: make sure the assets directory is valid
			return "assets/cache/shader/vulkan";
		}

		static void CreateCacheDirectoryIfNeeded()
		{
			std::string cacheDirectory = GetCacheDirectory();
			if (!std::filesystem::exists(cacheDirectory))
				std::filesystem::create_directories(cacheDirectory);
		}
	}
	
	VulkanShader::VulkanShader(const std::string& filepath)
		: m_FilePath(filepath)
	{
		ENGINE_PROFILE_FUNCTION();

		Utils::CreateCacheDirectoryIfNeeded();
		
		std::string source = ReadFile(filepath);
		//auto shaderSources = PreProcess(source);

		{
			Timer timer;
			//CompileOrGetVulkanBinaries(shaderSources);
			CompileOrGetOpenGLBinaries();
			CreateProgram();
			ENGINE_CORE_WARN("Shader creation took {0} ms", timer.ElapsedMillis());
		}

		// Extract name from filepath
		auto lastSlash = filepath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = filepath.rfind('.');
		auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
		m_Name = filepath.substr(lastSlash, count);
	}

	VulkanShader::VulkanShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
		: m_Name(name)
	{
		ENGINE_PROFILE_FUNCTION();
		
		//std::unordered_map<GLenum, std::string> shaderSources;
		//shaderSources[GL_VERTEX_SHADER] = vertexSrc;
		//shaderSources[GL_FRAGMENT_SHADER] = fragmentSrc;

		{
			Timer timer;
			//CompileOrGetVulkanBinaries(shaderSources);
			CompileOrGetOpenGLBinaries();
			CreateProgram();
			ENGINE_CORE_WARN("Shader creation took {0} ms", timer.ElapsedMillis());
		}
	}

	VulkanShader::~VulkanShader()
	{
		ENGINE_PROFILE_FUNCTION();
	}

	std::string VulkanShader::ReadFile(const std::string& filepath)
	{
		ENGINE_PROFILE_FUNCTION();
		
		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			size_t size = in.tellg();
			if (size != -1)
			{
				result.resize(size);
				in.seekg(0, std::ios::beg);
				in.read(&result[0], size);
				in.close();
			}
			else
			{
				ENGINE_CORE_ERROR("Could not read from file '{0}'", filepath);
			}
		}
		else
		{
			ENGINE_CORE_ERROR("Could not open file '{0}'", filepath);
		}
		
		return result;
	}

	/*
	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
	{
		ENGINE_PROFILE_FUNCTION();
		
		std::unordered_map<GLenum, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			ENGINE_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);
			ENGINE_CORE_ASSERT(Utils::ShaderTypeFromString(type), "Invalid shader type specified!");
			
			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			ENGINE_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");
			pos = source.find(typeToken, nextLinePos);

			shaderSources[Utils::ShaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
		}

		return shaderSources;
	}
	*/

	/*
	void VulkanShader::CompileOrGetVulkanBinaries(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
		const bool optimize = false;
		if (optimize) options.SetOptimizationLevel(shaderc_optimization_level_performance);

		std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();

		auto& shaderData = m_VulkanSPIRV;
		shaderData.clear();
		for (auto&& [stage, source] : shaderSources)
		{
			std::filesystem::path shaderFilePath = m_FilePath;
			std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + Utils::GLShaderStageCachedVulkanFileExtension(stage));

			std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
			if (in.is_open())
			{
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);

				auto& data = shaderData[stage];
				data.resize(size / sizeof(uint32_t));
				in.read((char*)data.data(), size);
			}
			else
			{
				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(stage), m_FilePath.c_str(), options);
				if (module.GetCompilationStatus() != shaderc_compilation_status_success)
				{
					ENGINE_CORE_ERROR(module.GetErrorMessage());
					ENGINE_CORE_ASSERT(false, "Shader Compilation Failed.");
				}

				shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

				std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					auto& data = shaderData[stage];
					out.write((char*)data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
				}
			}
		}

		for (auto&& [stage, data]: shaderData)
			Reflect(stage, data);
	}
	*/

	void VulkanShader::CompileOrGetOpenGLBinaries()
	{
		/*
		auto& shaderData = m_OpenGLSPIRV;

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
		const bool optimize = false;
		if (optimize) options.SetOptimizationLevel(shaderc_optimization_level_performance);

		std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();

		shaderData.clear();
		m_OpenGLSourceCode.clear();
		for (auto&& [stage, spirv] : m_VulkanSPIRV)
		{
			std::filesystem::path shaderFilePath = m_FilePath;
			std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + Utils::GLShaderStageCachedOpenGLFileExtension(stage));

			std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
			if (in.is_open())
			{
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);

				auto& data = shaderData[stage];
				data.resize(size / sizeof(uint32_t));
				in.read((char*)data.data(), size);
			}
			else
			{
				spirv_cross::CompilerGLSL glslCompiler(spirv);
				m_OpenGLSourceCode[stage] = glslCompiler.compile();
				auto& source = m_OpenGLSourceCode[stage];
				
				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(stage), m_FilePath.c_str(), options);
				if (module.GetCompilationStatus() != shaderc_compilation_status_success)
				{
					ENGINE_CORE_ERROR(module.GetErrorMessage());
					ENGINE_CORE_ASSERT(false, "Shader Compilation Failed.")
				}

				shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

				std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					auto& data = shaderData[stage];
					out.write((char*)data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
				}
			}
		}
		*/
	}

	void VulkanShader::CreateProgram()
	{
		/*
		GLuint program = glCreateProgram();

		std::vector<GLuint> shaderIDs;
		for (auto&& [stage, spirv] : m_OpenGLSPIRV)
		{
			GLuint shaderID = shaderIDs.emplace_back(glCreateShader(stage));
			glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(), spirv.size() * sizeof(uint32_t));
			glSpecializeShader(shaderID, "main", 0, nullptr, nullptr);
			glAttachShader(program, shaderID);
		}

		glLinkProgram(program);

		GLint isLinked;
		glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());
			ENGINE_CORE_ERROR("Shader linking failed ({0}):\n{1}", m_FilePath, infoLog.data());

			glDeleteProgram(program);

			for (auto id : shaderIDs)
				glDeleteShader(id);
		}
		else
		{
			for (auto id : shaderIDs)
			{
				glDetachShader(program, id);
				glDeleteShader(id);
			}
		}

		m_RenderID = program;
		*/
	}
	
	/*
	void VulkanShader::Reflect(GLenum stage, const std::vector<uint32_t>& shaderData)
	{
		spirv_cross::Compiler compiler(shaderData);
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		ENGINE_CORE_TRACE("OpenGLShader::Reflect - {0} {1}", Utils::GLShaderStageToString(stage), m_FilePath);
		ENGINE_CORE_TRACE("		{0} uniform buffers", resources.uniform_buffers.size());
		ENGINE_CORE_TRACE("		{0} resources", resources.sampled_images.size());

		ENGINE_CORE_TRACE("Uniform buffers:");
		for (const auto& resource : resources.uniform_buffers)
		{
			const auto& bufferType = compiler.get_type(resource.base_type_id);
			uint32_t bufferSize = compiler.get_declared_struct_size(bufferType);
			uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			int memberCount = bufferType.member_types.size();
				
			ENGINE_CORE_TRACE("	{0}", resource.name);
			ENGINE_CORE_TRACE("	Size = {0}", bufferSize);
			ENGINE_CORE_TRACE("	Binding = {0}", binding);
			ENGINE_CORE_TRACE("	Members = {0}", memberCount);
		}
	}
	*/

	void VulkanShader::Bind() const
	{
		ENGINE_PROFILE_FUNCTION();
	}

	void VulkanShader::Unbind() const
	{
		ENGINE_PROFILE_FUNCTION();
	}

	void VulkanShader::SetInt(const std::string& name, int value)
	{
		ENGINE_PROFILE_FUNCTION();
		
		UploadUniformInt(name, value);
	}

	void VulkanShader::SetIntArray(const std::string& name, int* values, uint32_t count)
	{
		ENGINE_PROFILE_FUNCTION();
		
		UploadUniformIntArray(name, values, count);
	}

	void VulkanShader::SetFloat(const std::string& name, const float value)
	{
		ENGINE_PROFILE_FUNCTION();
		
		UploadUniformFloat(name, value);
	}

	void VulkanShader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		ENGINE_PROFILE_FUNCTION();
		
		UploadUniformFloat3(name, value);
	}

	void VulkanShader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		ENGINE_PROFILE_FUNCTION();
		
		UploadUniformFloat4(name, value);
	}

	void VulkanShader::SetMat4(const std::string& name, const glm::mat4& value)
	{
		ENGINE_PROFILE_FUNCTION();
		
		UploadUniformMat4(name, value);
	}

	void VulkanShader::UploadUniformInt(const std::string& name, const int value)
	{
	}

	void VulkanShader::UploadUniformIntArray(const std::string& name, const int* values, uint32_t count)
	{
	}

	void VulkanShader::UploadUniformFloat(const std::string& name, const float value)
	{
	}

	void VulkanShader::UploadUniformFloat2(const std::string& name, const glm::vec2& values)
	{
	}

	void VulkanShader::UploadUniformFloat3(const std::string& name, const glm::vec3& values)
	{
	}

	void VulkanShader::UploadUniformFloat4(const std::string& name, const glm::vec4& values)
	{
	}

	void VulkanShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
	{
	}
	
	void VulkanShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
	}
}

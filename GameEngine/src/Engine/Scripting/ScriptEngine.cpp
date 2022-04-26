#include "enginepch.h"
#include "Engine/Scripting/ScriptEngine.h"

#include "Engine/Scripting/ScriptGlue.h"

namespace Engine
{
	ScriptEngine* ScriptEngine::s_Instance = nullptr;

	ScriptEngine::ScriptEngine()
	{
		ENGINE_PROFILE_FUNCTION();

		ENGINE_ASSERT(!s_Instance, "ScriptEngine already exists!");
		s_Instance = this;

		// Mono
		mono_set_dirs(R"(..\GameEngine\vendor\Mono\lib)", R"(..\GameEngine\vendor\Mono\etc)");

		m_MonoDomain = mono_jit_init("Engine-ScriptCore");
		ENGINE_CORE_ASSERT(m_MonoDomain, "Mono Domain could not be initialized!")

		MonoImageOpenStatus status = MONO_IMAGE_OK;

#ifdef ENGINE_DEBUG
		m_MonoAssembly = mono_assembly_open(R"(..\bin\Debug-windows-x86_64\Engine-ScriptCore\Engine-ScriptCore.dll)", &status);
#elif ENGINE_RELEASE
		m_MonoAssembly = mono_assembly_open(R"(..\bin\Release-windows-x86_64\Engine-ScriptCore\Engine-ScriptCore.dll)", &status);
#elif ENGINE_DIST
		m_MonoAssembly = mono_assembly_open(R"(..\bin\Dist-windows-x86_64\Engine-ScriptCore\Engine-ScriptCore.dll)", &status);
#endif

		switch (status)
		{
		case MONO_IMAGE_OK:
			ENGINE_CORE_TRACE("Mono Assembly: MONO_IMAGE_OK!");
			break;
		case MONO_IMAGE_ERROR_ERRNO:
			ENGINE_CORE_CRITICAL("Mono Assembly: MONO_IMAGE_ERROR_ERRNO!");
			break;
		case MONO_IMAGE_MISSING_ASSEMBLYREF:
			ENGINE_CORE_CRITICAL("Mono Assembly: MONO_IMAGE_MISSING_ASSEMBLYREF!");
			break;
		case MONO_IMAGE_IMAGE_INVALID:
			ENGINE_CORE_CRITICAL("Mono Assembly: MONO_IMAGE_IMAGE_INVALID!");
			break;
		}

		m_MonoAssemblyImage = mono_assembly_get_image(m_MonoAssembly);
		ENGINE_CORE_ASSERT(m_MonoAssemblyImage, "Mono Image could not be set!")

		InternalCalls::ScriptGlue::RegisterInternalCalls();
	}

	ScriptEngine::~ScriptEngine()
	{
		ENGINE_PROFILE_FUNCTION();

		// Release Domain
		if (m_MonoDomain)
		{
			mono_jit_cleanup(m_MonoDomain);
		}
	}

	void ScriptEngine::HandleMonoException(MonoObject* ptrExObject)
	{
		// Report Exception
		if (ptrExObject)
		{
			MonoString* exString = mono_object_to_string(ptrExObject, nullptr);
			const char* exCString = mono_string_to_utf8(exString);
			ENGINE_CORE_ERROR(exCString);
		}
	}

}

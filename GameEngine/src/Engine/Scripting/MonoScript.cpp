#include "enginepch.h"
#include "Engine/Scripting/MonoScript.h"
#include "Engine/Scripting/ScriptEngine.h"

#include <mono/metadata/object.h>

namespace Engine
{
	MonoScript::MonoScript(const std::string& namespaceName, const std::string& className)
	{
		ENGINE_CORE_ASSERT(namespaceName.c_str(), "Namespace is not set for mono script!");
		ENGINE_CORE_ASSERT(className.c_str(), "Class is not set for mono script!");

		MonoClass* ptrClass = mono_class_from_name(ScriptEngine::s_Instance->m_MonoAssemblyImage, namespaceName.c_str(), className.c_str());
		ENGINE_CORE_ASSERT(ptrClass, "Could not find class in mono assembly image!");

		m_PtrGameObject = mono_object_new(ScriptEngine::s_Instance->m_MonoDomain, ptrClass);
		ENGINE_CORE_ASSERT(m_PtrGameObject, "Could not create new MonoObject!");
		m_GameObjectGCHandle = mono_gchandle_new(m_PtrGameObject, false);

		// setup onCreate method
		MonoMethodDesc* ptrCreateMethodDesc = mono_method_desc_new(("." + className + ":OnCreate()").c_str(), false);
		ENGINE_CORE_ASSERT(ptrCreateMethodDesc, "Could create mono method desc!");

		m_OnCreateMethodPtr = mono_method_desc_search_in_class(ptrCreateMethodDesc, ptrClass);
		ENGINE_CORE_ASSERT(m_OnCreateMethodPtr, "Could not find create method desc in class!");

		mono_method_desc_free(ptrCreateMethodDesc);

		// setup onDestroy method
		MonoMethodDesc* ptrDestroyMethodDesc = mono_method_desc_new(("." + className + ":OnDestroy()").c_str(), false);
		ENGINE_CORE_ASSERT(ptrDestroyMethodDesc, "Could create mono method desc!");

		m_OnDestroyMethodPtr = mono_method_desc_search_in_class(ptrDestroyMethodDesc, ptrClass);
		ENGINE_CORE_ASSERT(m_OnDestroyMethodPtr, "Could not find destroy method desc in class!");

		mono_method_desc_free(ptrDestroyMethodDesc);

		// setup onUpdate method
		MonoMethodDesc* ptrUpdateMethodDesc = mono_method_desc_new(("." + className + ":OnUpdate(single)").c_str(), false);
		ENGINE_CORE_ASSERT(ptrUpdateMethodDesc, "Could create mono method desc!");

		m_OnUpdateMethodPtr = mono_method_desc_search_in_class(ptrUpdateMethodDesc, ptrClass);
		ENGINE_CORE_ASSERT(m_OnUpdateMethodPtr, "Could not find update method desc in class!");

		mono_method_desc_free(ptrUpdateMethodDesc);
	}

	MonoScript::~MonoScript()
	{
		if (m_GameObjectGCHandle)
		{
			mono_gchandle_free(m_GameObjectGCHandle);
		}
	}

	void MonoScript::OnCreate()
	{
		ENGINE_CORE_ASSERT(m_OnCreateMethodPtr, "On Create Method has not been set!");

		MonoObject* ptrExObject = nullptr;
		mono_runtime_invoke(m_OnCreateMethodPtr, m_PtrGameObject, nullptr, &ptrExObject);

		ScriptEngine::s_Instance->HandleMonoException(ptrExObject);
	}

	void MonoScript::OnDestroy()
	{
		ENGINE_CORE_ASSERT(m_OnDestroyMethodPtr, "On Destroy Method has not been set!");

		MonoObject* ptrExObject = nullptr;
		mono_runtime_invoke(m_OnDestroyMethodPtr, m_PtrGameObject, nullptr, &ptrExObject);

		ScriptEngine::s_Instance->HandleMonoException(ptrExObject);
	}

	void MonoScript::OnUpdate(Timestep ts)
	{
		ENGINE_CORE_ASSERT(m_OnUpdateMethodPtr, "On Update Method has not been set!");

		void* args[1]{};
		args[0] = &ts;

		MonoObject* ptrExObject = nullptr;
		mono_runtime_invoke(m_OnUpdateMethodPtr, m_PtrGameObject, args, &ptrExObject);

		ScriptEngine::s_Instance->HandleMonoException(ptrExObject);
	}
}

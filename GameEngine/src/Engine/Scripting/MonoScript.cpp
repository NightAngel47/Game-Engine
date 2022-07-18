
#include "enginepch.h"
#include "Engine/Scripting/MonoScript.h"
#include "Engine/Scripting/ScriptEngine.h"
#include "Engine/Scene/Entity.h"

#include <mono/metadata/object.h>

namespace Engine
{
	MonoScript::MonoScript(const std::string& scriptName)
	{
		std::string scriptNamespace, scriptClass;

		std::size_t scriptNameSplit = scriptName.find_last_of(".");
		if (scriptNameSplit == std::string::npos)
		{
			scriptNamespace = "null";
			scriptClass = scriptName;
		}
		else
		{
			scriptNamespace = scriptName.substr(0, scriptNameSplit);
			scriptClass = scriptName.substr(scriptNameSplit + 1);
			if (scriptClass.empty())
			{
				scriptClass = "null";
			}
		}

		auto image = ScriptEngine::s_Instance->GetMonoImage();
		ENGINE_CORE_ASSERT(image, "Mono Image not set!");
		MonoClass* ptrClass = mono_class_from_name(image, scriptNamespace.c_str(), scriptClass.c_str());
		if (!ptrClass)
		{
			ENGINE_CORE_ERROR("Could not find: " + scriptName + " in mono assembly image!");
		}
	}

	MonoScript::MonoScript(const std::string& scriptNamespace, const std::string& scriptClass)
	{
		auto image = ScriptEngine::s_Instance->GetMonoImage();
		ENGINE_CORE_ASSERT(image, "Mono Image not set!");
		MonoClass* ptrClass = mono_class_from_name(image, scriptNamespace.c_str(), scriptClass.c_str());
		if (!ptrClass)
		{
			ENGINE_CORE_ERROR("Could not find: " + scriptNamespace + "." + scriptClass + " in mono assembly image!");
		}
	}

	void MonoScript::InstantiateScript(const std::string& scriptName, Entity& entity)
	{
		std::string scriptNamespace, scriptClass;

		std::size_t scriptNameSplit = scriptName.find_last_of(".");
		if (scriptNameSplit == std::string::npos)
		{
			scriptNamespace = "null";
			scriptClass = scriptName;
		}
		else
		{
			scriptNamespace = scriptName.substr(0, scriptNameSplit);
			scriptClass = scriptName.substr(scriptNameSplit + 1);
			if (scriptClass.empty())
			{
				scriptClass = "null";
			}
		}

		InstantiateScript(scriptNamespace, scriptClass, entity);
	}

	void MonoScript::InstantiateScript(const std::string& namespaceName, const std::string& className, Entity& entity)
	{
		ENGINE_CORE_ASSERT(&namespaceName, "Script namespace is not set!");
		ENGINE_CORE_ASSERT(&className, "Script class is not set!");

		auto image = ScriptEngine::s_Instance->GetMonoImage();
		ENGINE_CORE_ASSERT(image, "Mono Image not set!");
		MonoClass* ptrClass = mono_class_from_name(image, namespaceName.c_str(), className.c_str());
		ENGINE_CORE_ASSERT(ptrClass, "Could not find class in mono assembly image!");

		auto domain = ScriptEngine::s_Instance->GetMonoDomain();
		ENGINE_CORE_ASSERT(domain, "Mono Domain not set!");
		m_PtrGameObject = mono_object_new(domain, ptrClass);
		ENGINE_CORE_ASSERT(m_PtrGameObject, "Could not create new MonoObject!");
		m_GameObjectGCHandle = mono_gchandle_new(m_PtrGameObject, false);

		// set EntityID property
		{
			MonoProperty* ptrIDProperty = mono_class_get_property_from_name(ptrClass, "ID");
			ENGINE_CORE_ASSERT(ptrIDProperty, "Could not find mono property!");

			MonoObject* ptrExObject = nullptr;
			void* params = nullptr;
			auto uuid = entity.GetUUID();
			ENGINE_CORE_TRACE("Instatiating " + namespaceName + "." + className + " for EntityID: " + std::to_string((uint64_t)uuid));
			params = &uuid;
			mono_property_set_value(ptrIDProperty, m_PtrGameObject, &params, &ptrExObject);
			ScriptEngine::HandleMonoException(ptrExObject);
		}

		// run Entity Constructor
		{
			MonoMethodDesc* ptrConstructorMethodDesc = mono_method_desc_new(":.ctor()", false);
			ENGINE_CORE_ASSERT(ptrConstructorMethodDesc, "Could create mono method desc!");

			MonoMethod* m_OnConstructorMethodPtr = mono_method_desc_search_in_class(ptrConstructorMethodDesc, ptrClass);
			ENGINE_CORE_ASSERT(m_OnConstructorMethodPtr, "Could not find create method desc in class!");

			mono_method_desc_free(ptrConstructorMethodDesc);

			MonoObject* ptrExObject = nullptr;
			mono_runtime_invoke(m_OnConstructorMethodPtr, m_PtrGameObject, nullptr, &ptrExObject);

			ScriptEngine::HandleMonoException(ptrExObject);
		}

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

	void MonoScript::OnCreate() // todo create wrapper for mono_rumtime_invoke that handles exceptions
	{
		ENGINE_CORE_ASSERT(m_OnCreateMethodPtr, "On Create Method has not been set!");

		MonoObject* ptrExObject = nullptr;
		mono_runtime_invoke(m_OnCreateMethodPtr, m_PtrGameObject, nullptr, &ptrExObject);

		ScriptEngine::HandleMonoException(ptrExObject);
	}

	void MonoScript::OnDestroy()
	{
		ENGINE_CORE_ASSERT(m_OnDestroyMethodPtr, "On Destroy Method has not been set!");

		MonoObject* ptrExObject = nullptr;
		mono_runtime_invoke(m_OnDestroyMethodPtr, m_PtrGameObject, nullptr, &ptrExObject);

		ScriptEngine::HandleMonoException(ptrExObject);
	}

	void MonoScript::OnUpdate(Timestep ts)
	{
		ENGINE_CORE_ASSERT(m_OnUpdateMethodPtr, "On Update Method has not been set!");

		void* args[1]{};
		args[0] = &ts;

		MonoObject* ptrExObject = nullptr;
		mono_runtime_invoke(m_OnUpdateMethodPtr, m_PtrGameObject, args, &ptrExObject);

		ScriptEngine::HandleMonoException(ptrExObject);
	}
}

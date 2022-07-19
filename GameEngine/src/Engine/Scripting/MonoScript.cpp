
#include "enginepch.h"
#include "Engine/Scripting/MonoScript.h"
#include "Engine/Scripting/ScriptEngine.h"
#include "Engine/Scene/Entity.h"

#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>
#include <mono/metadata/debug-helpers.h>

namespace Engine
{
	struct ScriptName
	{
		std::string scriptNamespace;
		std::string scriptClass;
	};


	static ScriptName SplitScriptName(const std::string& scriptName)
	{
		ScriptName script{};

		std::size_t scriptNameSplit = scriptName.find_last_of(".");
		if (scriptNameSplit == std::string::npos)
		{
			script.scriptNamespace = "null";
			script.scriptClass = scriptName;
		}
		else
		{
			script.scriptNamespace = scriptName.substr(0, scriptNameSplit);
			script.scriptClass = scriptName.substr(scriptNameSplit + 1);
			if (script.scriptClass.empty())
			{
				script.scriptClass = "null";
			}
		}

		return script;
	}

	MonoScript::MonoScript(const std::string& scriptName)
	{
		ScriptName script = SplitScriptName(scriptName);

		MonoClass* ptrClass = ScriptEngine::s_Instance->GetClassInAssembly(ScriptEngine::s_Instance->GetMonoAssembly(), script.scriptNamespace.c_str(), script.scriptClass.c_str());
	}

	MonoScript::MonoScript(const std::string& scriptNamespace, const std::string& scriptClass)
	{
		MonoClass* ptrClass = ScriptEngine::s_Instance->GetClassInAssembly(ScriptEngine::s_Instance->GetMonoAssembly(), scriptNamespace.c_str(), scriptClass.c_str());
	}

	void MonoScript::InstantiateScript(const std::string& scriptName, Entity& entity)
	{
		ScriptName script = SplitScriptName(scriptName);

		InstantiateScript(script.scriptNamespace, script.scriptClass, entity);
	}

	void MonoScript::InstantiateScript(const std::string& namespaceName, const std::string& className, Entity& entity)
	{
		ENGINE_CORE_ASSERT(&namespaceName, "Script namespace is not set!");
		ENGINE_CORE_ASSERT(&className, "Script class is not set!");

		MonoClass* ptrClass = ScriptEngine::s_Instance->GetClassInAssembly(ScriptEngine::s_Instance->GetMonoAssembly(), namespaceName.c_str(), className.c_str());

		m_ScriptInstance = mono_object_new(ScriptEngine::s_Instance->GetAppDomain(), ptrClass);
		ENGINE_CORE_ASSERT(m_ScriptInstance, "Could not create new MonoObject!");

		// run Entity parameterless (default) constructor
		mono_runtime_object_init(m_ScriptInstance);

		// set EntityID property
		{
			MonoProperty* ptrIDProperty = mono_class_get_property_from_name(ptrClass, "ID");
			ENGINE_CORE_ASSERT(ptrIDProperty, "Could not find mono property!");
		
			MonoObject* ptrExObject = nullptr;
			void* params = nullptr;
			auto uuid = entity.GetUUID();
			ENGINE_CORE_TRACE("Instatiating " + namespaceName + "." + className + " for EntityID: " + std::to_string((uint64_t)uuid));
			params = &uuid;
			mono_property_set_value(ptrIDProperty, m_ScriptInstance, &params, &ptrExObject);
			ScriptEngine::s_Instance->HandleMonoException(ptrExObject);
		}

		// setup onCreate method
		m_OnCreateMethodPtr = mono_class_get_method_from_name(ptrClass, "OnCreate", 0);
		ENGINE_CORE_ASSERT(m_OnCreateMethodPtr, "Could not find create method desc in class!");

		// setup onDestroy method
		m_OnDestroyMethodPtr = mono_class_get_method_from_name(ptrClass, "OnDestroy", 0);
		ENGINE_CORE_ASSERT(m_OnDestroyMethodPtr, "Could not find destroy method desc in class!");

		// setup onUpdate method
		m_OnUpdateMethodPtr = mono_class_get_method_from_name(ptrClass, "OnUpdate", 1);
		ENGINE_CORE_ASSERT(m_OnUpdateMethodPtr, "Could not find update method desc in class!");
	}

	MonoScript::~MonoScript()
	{

	}

	void MonoScript::OnCreate() // todo create wrapper for mono_rumtime_invoke that handles exceptions
	{
		ENGINE_CORE_ASSERT(m_OnCreateMethodPtr, "On Create Method has not been set!");

		MonoObject* ptrExObject = nullptr;
		mono_runtime_invoke(m_OnCreateMethodPtr, m_ScriptInstance, nullptr, &ptrExObject);

		ScriptEngine::s_Instance->HandleMonoException(ptrExObject);
	}

	void MonoScript::OnDestroy()
	{
		ENGINE_CORE_ASSERT(m_OnDestroyMethodPtr, "On Destroy Method has not been set!");

		MonoObject* ptrExObject = nullptr;
		mono_runtime_invoke(m_OnDestroyMethodPtr, m_ScriptInstance, nullptr, &ptrExObject);

		ScriptEngine::s_Instance->HandleMonoException(ptrExObject);
	}

	void MonoScript::OnUpdate(Timestep ts)
	{
		ENGINE_CORE_ASSERT(m_OnUpdateMethodPtr, "On Update Method has not been set!");

		void* param = &ts;

		MonoObject* ptrExObject = nullptr;
		mono_runtime_invoke(m_OnUpdateMethodPtr, m_ScriptInstance, &param, &ptrExObject);

		ScriptEngine::s_Instance->HandleMonoException(ptrExObject);
	}
}

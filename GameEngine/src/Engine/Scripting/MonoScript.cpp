
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

		m_ScriptClass = ScriptEngine::s_Instance->GetClassInAssembly(ScriptEngine::s_Instance->GetMonoAssembly(), script.scriptNamespace.c_str(), script.scriptClass.c_str());
	}

	MonoScript::MonoScript(const std::string& scriptNamespace, const std::string& scriptClass)
	{
		m_ScriptClass = ScriptEngine::s_Instance->GetClassInAssembly(ScriptEngine::s_Instance->GetMonoAssembly(), scriptNamespace.c_str(), scriptClass.c_str());
	}

	void MonoScript::InstantiateScript(Entity& entity)
	{
		m_ScriptInstance = mono_object_new(ScriptEngine::s_Instance->GetAppDomain(), m_ScriptClass);
		ENGINE_CORE_ASSERT(m_ScriptInstance, "Could not create new MonoObject!");

		// run Entity parameterless (default) constructor
		mono_runtime_object_init(m_ScriptInstance);

		// set EntityID property
		{
			MonoProperty* ptrIDProperty = mono_class_get_property_from_name(m_ScriptClass, "ID");
			ENGINE_CORE_ASSERT(ptrIDProperty, "Could not find mono property!");
		
			MonoObject* ptrExObject = nullptr;
			void* params = nullptr;
			auto uuid = entity.GetUUID();
			params = &uuid;
			mono_property_set_value(ptrIDProperty, m_ScriptInstance, &params, &ptrExObject);
			ScriptEngine::s_Instance->HandleMonoException(ptrExObject);
		}

		// setup onCreate method
		MonoMethod* OnCreateMethodPtr = mono_class_get_method_from_name(m_ScriptClass, "OnCreate", 0);
		ENGINE_CORE_ASSERT(OnCreateMethodPtr, "Could not find create method desc in class!");
		OnCreateThunk = (OnCreate)mono_method_get_unmanaged_thunk(OnCreateMethodPtr);

		// setup onDestroy method
		MonoMethod* OnDestroyMethodPtr = mono_class_get_method_from_name(m_ScriptClass, "OnDestroy", 0);
		ENGINE_CORE_ASSERT(OnDestroyMethodPtr, "Could not find destroy method desc in class!");
		OnDestroyThunk = (OnDestroy)mono_method_get_unmanaged_thunk(OnDestroyMethodPtr);

		// setup onUpdate method
		MonoMethod* OnUpdateMethodPtr = mono_class_get_method_from_name(m_ScriptClass, "OnUpdate", 1);
		ENGINE_CORE_ASSERT(OnUpdateMethodPtr, "Could not find update method desc in class!");
		OnUpdateThunk = (OnUpdate)mono_method_get_unmanaged_thunk(OnUpdateMethodPtr);

		m_Timestep = ScriptEngine::s_Instance->GetClassInAssembly(ScriptEngine::s_Instance->GetMonoAssembly(), "Engine.Core", "Timestep");
	}

	MonoScript::~MonoScript()
	{

	}

	void MonoScript::OnCreateMethod()
	{
		MonoObject* ptrExObject = nullptr;

		OnCreateThunk(m_ScriptInstance, &ptrExObject);

		ScriptEngine::s_Instance->HandleMonoException(ptrExObject);
	}

	void MonoScript::OnDestroyMethod()
	{
		MonoObject* ptrExObject = nullptr;

		OnDestroyThunk(m_ScriptInstance, &ptrExObject);

		ScriptEngine::s_Instance->HandleMonoException(ptrExObject);
	}

	void MonoScript::OnUpdateMethod(Timestep ts)
	{
		MonoObject* paramBox = mono_value_box(ScriptEngine::s_Instance->GetAppDomain(), m_Timestep, &ts);

		MonoObject* ptrExObject = nullptr;

		OnUpdateThunk(m_ScriptInstance, paramBox, &ptrExObject);

		ScriptEngine::s_Instance->HandleMonoException(ptrExObject);
	}
}

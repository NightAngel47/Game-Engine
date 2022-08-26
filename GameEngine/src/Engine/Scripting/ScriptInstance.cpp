#include "enginepch.h"
#include "Engine/Scripting/ScriptInstance.h"

#include <mono/metadata/object.h>

namespace Engine
{
	ScriptInstance::ScriptInstance(Ref<ScriptClass> scriptClass, const UUID& entityID)
		: m_ScriptClass(scriptClass)
	{
		m_Instance = m_ScriptClass->Instantiate();

		m_Constructor = mono_class_get_method_from_name(ScriptEngine::GetEntityClass(), ".ctor", 1);
		{
			UUID id = entityID;
			void* param = &id;
			m_ScriptClass->InvokeMethod(m_Instance, m_Constructor, &param);
		}

		MonoClass* monoClass = m_ScriptClass->GetMonoClass();

		// setup onCreate method
		MonoMethod* OnCreateMethodPtr = mono_class_get_method_from_name(monoClass, "OnCreate", 0);
		ENGINE_CORE_ASSERT(OnCreateMethodPtr, "Could not find create method desc in class!");
		OnCreateThunk = (OnCreate)mono_method_get_unmanaged_thunk(OnCreateMethodPtr);

		// setup onDestroy method
		MonoMethod* OnDestroyMethodPtr = mono_class_get_method_from_name(monoClass, "OnDestroy", 0);
		ENGINE_CORE_ASSERT(OnDestroyMethodPtr, "Could not find destroy method desc in class!");
		OnDestroyThunk = (OnDestroy)mono_method_get_unmanaged_thunk(OnDestroyMethodPtr);

		// setup onUpdate method
		MonoMethod* OnUpdateMethodPtr = mono_class_get_method_from_name(monoClass, "OnUpdate", 1);
		ENGINE_CORE_ASSERT(OnUpdateMethodPtr, "Could not find update method desc in class!");
		OnUpdateThunk = (OnUpdate)mono_method_get_unmanaged_thunk(OnUpdateMethodPtr);
	}

	void ScriptInstance::InvokeOnCreate()
	{
		if (!OnCreateThunk) return; // handle script without OnCreate

		MonoObject* ptrExObject = nullptr;
		OnCreateThunk(m_Instance, &ptrExObject);
		ScriptEngine::HandleMonoException(ptrExObject);
	}

	void ScriptInstance::InvokeOnDestroy()
	{
		if (!OnDestroyThunk) return; // handle script without OnDestroy

		MonoObject* ptrExObject = nullptr;
		OnDestroyThunk(m_Instance, &ptrExObject);
		ScriptEngine::HandleMonoException(ptrExObject);
	}

	void ScriptInstance::InvokeOnUpdate(Timestep ts)
	{
		if (!OnUpdateThunk) return; // handle script without OnUpdate

		MonoObject* paramBox = mono_value_box(ScriptEngine::GetAppDomain(), ScriptEngine::GetTimestepClass(), &ts);
		MonoObject* ptrExObject = nullptr;
		OnUpdateThunk(m_Instance, paramBox, &ptrExObject);
		ScriptEngine::HandleMonoException(ptrExObject);
	}
}

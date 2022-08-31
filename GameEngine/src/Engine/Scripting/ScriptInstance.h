#pragma once
#include "Engine/Scripting/ScriptEngine.h"
#include "Engine/Scripting/ScriptClass.h"
#include "Engine/Core/Timestep.h"

typedef void(*OnCreate) (MonoObject* obj, MonoObject** exp);
typedef void(*OnDestroy) (MonoObject* obj, MonoObject** exp);
typedef void(*OnUpdate) (MonoObject* obj, float* ts, MonoObject** exp);

namespace Engine
{

	class ScriptInstance
	{
	public:
		ScriptInstance() = default;
		ScriptInstance(Ref<ScriptClass> scriptClass, const UUID& entityID);
		~ScriptInstance() = default;

		void InvokeOnCreate();
		void InvokeOnDestroy();
		void InvokeOnUpdate(float ts);

		MonoObject* GetMonoObject() { return m_Instance; }

	private:
		Ref<ScriptClass> m_ScriptClass;

		MonoObject* m_Instance = nullptr;

		MonoMethod* m_Constructor = nullptr;
		OnCreate OnCreateThunk = nullptr;
		OnDestroy OnDestroyThunk = nullptr;
		OnUpdate OnUpdateThunk = nullptr;
	};
}


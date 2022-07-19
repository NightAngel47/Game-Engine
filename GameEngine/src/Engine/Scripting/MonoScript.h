#pragma once
#include "Engine/Core/Timestep.h"

#include <mono/metadata/object-forward.h>


namespace Engine
{
	// forward dec
	class Entity;

	typedef void(*OnCreate) (MonoObject* obj, MonoObject** exp);
	typedef void(*OnDestroy) (MonoObject* obj, MonoObject** exp);
	typedef void(*OnUpdate) (MonoObject* obj, MonoObject* timestep, MonoObject** exp);

	class MonoScript
	{
	public:
		MonoScript() = default;
		MonoScript(const std::string& scriptName);
		MonoScript(const std::string& scriptNamespace, const std::string& scriptClass);
		~MonoScript();

		void InstantiateScript(Entity& entity);

		void OnCreateMethod();
		void OnDestroyMethod();
		void OnUpdateMethod(Timestep ts);
	private:
		MonoObject* m_ScriptInstance = nullptr;
		MonoClass* m_ScriptClass = nullptr;
		MonoClass* m_Timestep = nullptr;

		OnCreate OnCreateThunk = nullptr;
		OnDestroy OnDestroyThunk = nullptr;
		OnUpdate OnUpdateThunk = nullptr;
	};
}

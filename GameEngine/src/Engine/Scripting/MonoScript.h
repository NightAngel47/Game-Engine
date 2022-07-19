#pragma once
#include "Engine/Core/Timestep.h"

#include <mono/metadata/object-forward.h>

namespace Engine
{
	// forward dec
	class Entity;

	class MonoScript
	{
	public:
		MonoScript() = default;
		MonoScript(const std::string& scriptName);
		MonoScript(const std::string& scriptNamespace, const std::string& scriptClass);
		~MonoScript();

		void InstantiateScript(const std::string& scriptName, Entity& entity);
		void InstantiateScript(const std::string& namespaceName, const std::string& className, Entity& entity);

		void OnCreate();
		void OnDestroy();
		void OnUpdate(Timestep ts);
	private:
		MonoMethod* m_OnCreateMethodPtr = nullptr;
		MonoMethod* m_OnDestroyMethodPtr = nullptr;
		MonoMethod* m_OnUpdateMethodPtr = nullptr;

		MonoObject* m_ScriptInstance = nullptr;
	};
}

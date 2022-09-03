#pragma once
#include "Engine/Scripting/ScriptField.h"

extern "C"
{
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
}

namespace Engine
{
	class ScriptClass
	{
	public:
		ScriptClass(const std::string& classNamespace, const std::string& className);
		~ScriptClass() = default;

		MonoObject* Instantiate();

		MonoClass* GetMonoClass() { return m_MonoClass; }

		MonoMethod* GetMethod(const std::string& name, int parameterCount);
		MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr);

		Ref<ScriptField> GetScriptField(const std::string& fieldName) { return m_ScriptFields.at(fieldName); }
		std::unordered_map<std::string, Ref<ScriptField>> GetScriptFields() { return m_ScriptFields; }

	private:
		std::string m_ClassNamespace;
		std::string m_ClassName;

		MonoClass* m_MonoClass = nullptr;
		std::unordered_map<std::string, Ref<ScriptField>> m_ScriptFields;
	};
}


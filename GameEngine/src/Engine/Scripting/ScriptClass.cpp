#include "enginepch.h"
#include "Engine/Scripting/ScriptClass.h"
#include "Engine/Scripting/ScriptEngine.h"

#include <mono/metadata/object.h>
#include <mono/metadata/debug-helpers.h>

namespace Engine
{
	ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className)
		: m_ClassNamespace(classNamespace), m_ClassName(className)
	{
		m_MonoClass = ScriptEngine::GetClassInAssembly(ScriptEngine::GetAppAssembly(), m_ClassNamespace.c_str(), m_ClassName.c_str());

		int i = 0;
		void* itr = nullptr;
		MonoClassField* field = nullptr;
		while ((field = mono_class_get_fields(m_MonoClass, &itr)) != nullptr)
		{
			const char* fieldName = mono_field_get_name(field);
			m_ScriptFields[fieldName] = new ScriptField(field);
			++i;
		}
	}

	MonoObject* ScriptClass::Instantiate()
	{
		return ScriptEngine::InstantiateClass(m_MonoClass);
	}

	MonoMethod* ScriptClass::GetMethod(const std::string& name, int parameterCount)
	{
		return mono_class_get_method_from_name(m_MonoClass, name.c_str(), parameterCount);
	}

	MonoObject* ScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** params)
	{
		MonoObject* monoException = nullptr;
		MonoObject* result = mono_runtime_invoke(method, instance, params, &monoException);

		ScriptEngine::HandleMonoException(monoException);

		return result;
	}
}

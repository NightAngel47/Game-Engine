#include "enginepch.h"
#include "Engine/Scripting/ScriptField.h"
#include "Engine/Scripting/ScriptEngine.h"

#include <mono/metadata/object.h>
#include <mono/metadata/debug-helpers.h>

namespace Engine
{
	ScriptField::ScriptField(MonoClassField* monoField)
		:m_MonoField(monoField)
	{
		m_Access = ScriptEngine::GetFieldAccessibility(m_MonoField);
		MonoType* monoType = mono_field_get_type(m_MonoField);
		m_TypeName = mono_type_get_name(monoType);

		// TODO remove/move to debug only
		std::string name = mono_field_get_name(m_MonoField);
		ENGINE_CORE_TRACE("Field: " + name + " Type: " + m_TypeName);
	}

	void ScriptField::GetValue(Ref<ScriptInstance> instance, void* value)
	{
		mono_field_get_value(instance->GetMonoObject(), m_MonoField, value);
	}

	void ScriptField::SetValue(Ref<ScriptInstance> instance, void* value)
	{
		mono_field_set_value(instance->GetMonoObject(), m_MonoField, value);
	}

	bool ScriptField::IsPublic()
	{
		return m_Access & (uint8_t)ScriptEngine::Accessibility::Public ? true : false;
	}
}

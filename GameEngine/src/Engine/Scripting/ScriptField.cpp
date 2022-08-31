#include "enginepch.h"
#include "Engine/Scripting/ScriptField.h"
#include "Engine/Scripting/ScriptEngine.h"

#include <mono/metadata/object.h>
#include <mono/metadata/debug-helpers.h>

namespace Engine
{
	static std::unordered_map<std::string, ScriptFieldType> s_ScriptFieldTypeMap =
	{
		{ "System.Single", ScriptFieldType::Float },
		{ "System.Double", ScriptFieldType::Double },
		{ "System.Boolean", ScriptFieldType::Bool },
		{ "System.Char", ScriptFieldType::Char },
		{ "System.String", ScriptFieldType::String },
		{ "System.Int16", ScriptFieldType::Short },
		{ "System.Int32", ScriptFieldType::Int },
		{ "System.Int64", ScriptFieldType::Long },
		{ "System.Byte", ScriptFieldType::Byte },
		{ "System.UInt16", ScriptFieldType::UShort },
		{ "System.UInt32", ScriptFieldType::UInt },
		{ "System.UInt64", ScriptFieldType::ULong },

		{ "Engine.Math.Vector2", ScriptFieldType::Vector2 },
		{ "Engine.Math.Vector3", ScriptFieldType::Vector3 },
		{ "Engine.Math.Vector4", ScriptFieldType::Vector4 },

		{ "Engine.Scene.Entity", ScriptFieldType::Entity }
	};

	ScriptField::ScriptField(MonoClassField* monoField)
		:m_MonoField(monoField)
	{
		m_Access = ScriptEngine::GetFieldAccessibility(m_MonoField);
		MonoType* monoType = mono_field_get_type(m_MonoField);
		std::string typeName = mono_type_get_name(monoType);

		if (s_ScriptFieldTypeMap.find(typeName) != s_ScriptFieldTypeMap.end())
		{
			m_Type = s_ScriptFieldTypeMap.at(typeName);
		}
		else
		{
			ENGINE_CORE_ERROR("Script Field Type: {} not supported!", typeName);
			m_Type = ScriptFieldType::None;
		}
	}

	const char* ScriptField::GetTypeName()
	{
		switch (m_Type)
		{
			case ScriptFieldType::Float:   return "Float";
			case ScriptFieldType::Double:  return "Double";
			case ScriptFieldType::Bool:    return "Bool";
			case ScriptFieldType::Char:    return "Char";
			case ScriptFieldType::String:  return "String";
			case ScriptFieldType::Byte:    return "Byte";
			case ScriptFieldType::Short:   return "Short";
			case ScriptFieldType::Int:     return "Int";
			case ScriptFieldType::Long:    return "Long";
			case ScriptFieldType::UByte:   return "UByte";
			case ScriptFieldType::UShort:  return "UShort";
			case ScriptFieldType::UInt:    return "UInt";
			case ScriptFieldType::ULong:   return "ULong";
			case ScriptFieldType::Vector2: return "Vector2";
			case ScriptFieldType::Vector3: return "Vector3";
			case ScriptFieldType::Vector4: return "Vector4";
			case ScriptFieldType::Entity:  return "Entity";
		}
		return "<Invalid>";
	}

	template<typename T>
	T ScriptField::GetValue(Ref<ScriptInstance> instance)
	{
		GetValueInternal(instance, s_FieldValueBuffer);
		return *(T*)s_FieldValueBuffer;
	}

	template<>
	float ScriptField::GetValue(Ref<ScriptInstance> instance)
	{
		GetValueInternal(instance, s_FieldValueBuffer);
		return *(float*)s_FieldValueBuffer;
	}

	template<>
	int ScriptField::GetValue(Ref<ScriptInstance> instance)
	{
		GetValueInternal(instance, s_FieldValueBuffer);
		return *(int*)s_FieldValueBuffer;
	}

	template<>
	MonoString* ScriptField::GetValue(Ref<ScriptInstance> instance)
	{
		GetValueInternal(instance, s_FieldValueBuffer);
		return *(MonoString**)s_FieldValueBuffer;
	}

	template<typename T>
	void ScriptField::SetValue(Ref<ScriptInstance> instance, T* value)
	{
		SetValueInternal(instance, value);
	}

	template<>
	void ScriptField::SetValue(Ref<ScriptInstance> instance, float* value)
	{
		SetValueInternal(instance, value);
	}

	template<>
	void ScriptField::SetValue(Ref<ScriptInstance> instance, int* value)
	{
		SetValueInternal(instance, value);
	}

	template<>
	void ScriptField::SetValue(Ref<ScriptInstance> instance, MonoString* value)
	{
		SetValueInternal(instance, value);
	}

	bool ScriptField::IsPublic()
	{
		return m_Access & (uint8_t)ScriptEngine::Accessibility::Public ? true : false;
	}

	void ScriptField::GetValueInternal(Ref<ScriptInstance> instance, void* buffer)
	{
		mono_field_get_value(instance->GetMonoObject(), m_MonoField, s_FieldValueBuffer);
	}

	void ScriptField::SetValueInternal(Ref<ScriptInstance> instance, const void* value)
	{
		mono_field_set_value(instance->GetMonoObject(), m_MonoField, (void*)value);
	}
}

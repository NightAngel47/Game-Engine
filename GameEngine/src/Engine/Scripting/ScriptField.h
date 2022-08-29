#pragma once
#include "Engine/Scripting/ScriptInstance.h"

namespace Engine
{
	enum class ScriptFieldType
	{
		None = -1,
		Float, Double,
		Bool, Char, String,
		Byte, Short, Int, Long,
		UByte, UShort, UInt, ULong,
		Vector2, Vector3, Vector4,
		Entity
	};

	class ScriptField
	{
	public:
		ScriptField() = default;
		ScriptField(MonoClassField* monoField);
		~ScriptField() = default;

		MonoClassField* GetMonoField() { return m_MonoField; }
		const ScriptFieldType GetType() { return m_Type; }
		const char* GetTypeName();

		template<typename T>
		T GetValue(Ref<ScriptInstance> instance);

		template<typename T>
		void SetValue(Ref<ScriptInstance> instance, T* value);

		bool IsPublic();

	private:
		void GetValueInternal(Ref<ScriptInstance> instance, void* buffer);
		void SetValueInternal(Ref<ScriptInstance> instance, const void* value);

	private:
		MonoClassField* m_MonoField = nullptr;
		uint8_t m_Access;
		ScriptFieldType m_Type;

		inline static char s_FieldValueBuffer[8];
	};
}


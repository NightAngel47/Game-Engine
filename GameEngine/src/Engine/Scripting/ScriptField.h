#pragma once

extern "C"
{
	typedef struct _MonoClassField MonoClassField;
	typedef struct _MonoObject MonoObject;
}

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
		T GetValue(MonoObject* instance);

		template<typename T>
		void SetValue(MonoObject* instance, T* value);

		bool IsPublic();

	private:
		void GetValueInternal(MonoObject* instance, void* buffer);
		void SetValueInternal(MonoObject* instance, const void* value);

	private:
		MonoClassField* m_MonoField = nullptr;
		uint8_t m_Access;
		ScriptFieldType m_Type;

		inline static char s_FieldValueBuffer[8];
	};
}


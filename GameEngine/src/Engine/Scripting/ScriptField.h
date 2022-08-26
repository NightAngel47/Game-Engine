#pragma once
#include "Engine/Scripting/ScriptInstance.h"

namespace Engine
{
	class ScriptField
	{
	public:
		ScriptField() = default;
		ScriptField(MonoClassField* monoField);
		~ScriptField() = default;

		MonoClassField* GetMonoField() { return m_MonoField; }
		const std::string& GetTypeName() { return m_TypeName; }
		void GetValue(Ref<ScriptInstance> instance, void* value);
		void SetValue(Ref<ScriptInstance> instance, void* value);

		bool IsPublic();

	private:
		MonoClassField* m_MonoField = nullptr;
		uint8_t m_Access;
		std::string m_TypeName;
	};
}


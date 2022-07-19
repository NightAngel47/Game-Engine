#pragma once

#include <mono/jit/jit.h>

// Created with help from this guide (Mono Embedding for Game Engines): https://peter1745.github.io/introduction.html

namespace Engine
{

	class ScriptEngine
	{
	public:
		ScriptEngine();
		~ScriptEngine();

		static ScriptEngine* s_Instance;

		MonoDomain* GetMonoDomain() { return m_MonoDomain; }
		MonoDomain* GetAppDomain() { return m_AppDomain; }
		MonoAssembly* GetMonoAssembly() { return m_MonoAssembly; }

		MonoClass* GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className);
		uint8_t GetFieldAccessibility(MonoClassField* field);
		uint8_t GetPropertyAccessbility(MonoProperty* property);

		void HandleMonoException(MonoObject* ptrExObject);
		bool CheckMonoError(MonoError& error);

		std::string MonoStringToUTF8(MonoString* monoString);

	private:
		MonoAssembly* LoadCSharpAssembly(const std::string& assemblyPath);
		void PrintAssemblyTypes(MonoAssembly* assembly);

	private:
		MonoDomain* m_MonoDomain;
		MonoDomain* m_AppDomain;
		MonoAssembly* m_MonoAssembly;
	};
}

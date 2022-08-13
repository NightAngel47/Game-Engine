#pragma once

extern "C"
{
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoDomain MonoDomain;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoClassField MonoClassField;
	typedef struct _MonoProperty MonoProperty;
	typedef struct _MonoString MonoString;
}

// Created with help from this guide (Mono Embedding for Game Engines): https://peter1745.github.io/introduction.html

namespace Engine
{

	class ScriptEngine
	{
	public:
		ScriptEngine() = delete;

		static void Init();
		static void Shutdown();

		static MonoDomain* GetRootDomain();
		static MonoDomain* GetAppDomain();
		static MonoAssembly* GetCoreAssembly();

		static MonoClass* GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className);
		static uint8_t GetFieldAccessibility(MonoClassField* field);
		static uint8_t GetPropertyAccessbility(MonoProperty* property);

		static void HandleMonoException(MonoObject* ptrExObject);

		static std::string MonoStringToUTF8(MonoString* monoString);

	private:
		static void InitMono();
		static void LoadAssembly(const std::filesystem::path& assemblyPath);
		static void ShutdownMono();
	};
}

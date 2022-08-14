#pragma once
#include "Engine/Core/Timestep.h"
#include "Engine/Scene/Scene.h"

extern "C"
{
	typedef struct _MonoDomain MonoDomain;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoClassField MonoClassField;
	typedef struct _MonoProperty MonoProperty;
	typedef struct _MonoString MonoString;
}

typedef void(*OnCreate) (MonoObject* obj, MonoObject** exp);
typedef void(*OnDestroy) (MonoObject* obj, MonoObject** exp);
typedef void(*OnUpdate) (MonoObject* obj, MonoObject* timestep, MonoObject** exp);

// Created with help from this guide (Mono Embedding for Game Engines): https://peter1745.github.io/introduction.html

namespace Engine
{

	class ScriptClass
	{
	public:
		ScriptClass(std::string classNamespace, std::string className);
		~ScriptClass() = default;

		MonoObject* Instantiate();

		MonoClass* GetMonoClass() { return m_MonoClass; }

		MonoMethod* GetMethod(const std::string& name, int parameterCount);
		MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr);

	private:
		std::string m_ClassNamespace;
		std::string m_ClassName;

		MonoClass* m_MonoClass = nullptr;
	};

	class ScriptInstance
	{
	public:
		ScriptInstance(Ref<ScriptClass> scriptClass);
		~ScriptInstance() = default;

		void InvokeOnCreate();
		void InvokeOnDestroy();
		void InvokeOnUpdate(Timestep ts);

		MonoObject* GetMonoObject() { return m_Instance; }

	private:
		Ref<ScriptClass> m_ScriptClass;

		MonoObject* m_Instance = nullptr;

		OnCreate OnCreateThunk = nullptr;
		OnDestroy OnDestroyThunk = nullptr;
		OnUpdate OnUpdateThunk = nullptr;
	};

	class ScriptEngine
	{
	public:
		ScriptEngine() = delete;

		static void Init();
		static void Shutdown();

		static void OnRuntimeStart(Scene* scene);
		static void OnRuntimeStop();

		static std::unordered_map<std::string, Ref<ScriptClass>> GetEntityClasses();
		static Scene* GetSceneContext();
		
		static bool EntityClassExists(const std::string& className);
		static void OnCreateEntity(Entity entity, const std::string& className);
		static void OnDestroyEntity(Entity entity, const std::string& className);
		static void OnUpdateEntity(Entity entity, const std::string& className, Timestep ts);

		static bool EntityInstanceExists(const UUID& entityID);
		static Ref<ScriptInstance> GetEntityInstance(const UUID& entityID);

		static MonoDomain* GetRootDomain();
		static MonoDomain* GetAppDomain();
		static MonoAssembly* GetCoreAssembly();

		static MonoClass* GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className);
		static uint8_t GetFieldAccessibility(MonoClassField* field);
		static uint8_t GetPropertyAccessbility(MonoProperty* property);

		static std::string MonoStringToUTF8(MonoString* monoString);

	private:
		static void InitMono();
		static void ShutdownMono();

		static void LoadAssembly(const std::filesystem::path& assemblyPath);
		static void LoadEntityClasses(MonoAssembly* assembly);
		static MonoObject* InstantiateClass(MonoClass* monoClass);

		friend class ScriptClass;
		friend class ScriptInstance;
	};
}

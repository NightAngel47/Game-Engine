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
	typedef struct _MonoType MonoType;
	typedef struct _MonoProperty MonoProperty;
	typedef struct _MonoString MonoString;
}

// Created with help from this guide (Mono Embedding for Game Engines): https://peter1745.github.io/introduction.html

namespace Engine
{
	// forward declaration
	class ScriptClass;
	class ScriptInstance;

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
		static Ref<ScriptInstance> CreateEntityInstance(const UUID& entityID, const std::string& className);
		static void DeleteEntityInstance(Ref<ScriptInstance> instance, UUID entityID);

		static void OnCreateEntity(Entity entity, const std::string& className);
		static void OnDestroyEntity(Entity entity, const std::string& className);
		static void OnUpdateEntity(Entity entity, const std::string& className, Timestep ts);

		static bool EntityInstanceExists(const UUID& entityID);
		static Ref<ScriptInstance> GetEntityInstance(const UUID& entityID);

		static MonoDomain* GetRootDomain();
		static MonoDomain* GetAppDomain();
		static MonoAssembly* GetCoreAssembly();
		static MonoAssembly* GetAppAssembly();
		static MonoClass* GetEntityClass();

		static MonoClass* GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className);
		static uint8_t GetFieldAccessibility(MonoClassField* field);
		static uint8_t GetPropertyAccessbility(MonoProperty* property);

		static void HandleMonoException(MonoObject* ptrExObject);
		static MonoString* StringToMonoString(std::string string);
		static std::string MonoStringToUTF8(MonoString* monoString);

		enum class Accessibility : uint8_t
		{
			None = 0,
			Private = (1 << 0),
			Internal = (1 << 1),
			Protected = (1 << 2),
			Public = (1 << 3)
		};

	private:
		static void InitMono();
		static void ShutdownMono();

		static void LoadCoreAssembly(const std::filesystem::path& assemblyPath);
		static void LoadAppAssembly(const std::filesystem::path& assemblyPath);
		static void LoadEntityClasses(MonoAssembly* assembly);
		static MonoObject* InstantiateClass(MonoClass* monoClass);

		friend class ScriptClass;
		friend class ScriptInstance;
	};
}

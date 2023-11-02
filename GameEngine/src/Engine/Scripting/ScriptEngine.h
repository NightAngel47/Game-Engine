#pragma once
#include "Engine/Core/Timestep.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Components.h"
#include "Engine/Physics/Physics2D.h"

extern "C"
{
	typedef struct _MonoDomain MonoDomain;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoArray MonoArray;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoClassField MonoClassField;
	typedef struct _MonoType MonoType;
	typedef struct _MonoProperty MonoProperty;
	typedef struct _MonoString MonoString;
}

typedef void(*OnCreate) (MonoObject* obj, MonoObject** exp);
typedef void(*OnStart) (MonoObject* obj, MonoObject** exp);
typedef void(*OnDestroy) (MonoObject* obj, MonoObject** exp);
typedef void(*OnUpdate) (MonoObject* obj, float* ts, MonoObject** exp);
typedef void(*OnLateUpdate) (MonoObject* obj, float* ts, MonoObject** exp);

typedef void(*OnTriggerEnter2D) (MonoObject* obj, MonoObject* physics2DContact, MonoObject** exp);
typedef void(*OnTriggerExit2D) (MonoObject* obj, MonoObject* physics2DContact, MonoObject** exp);
typedef void(*OnCollisionEnter2D) (MonoObject* obj, MonoObject* physics2DContact, MonoObject** exp);
typedef void(*OnCollisionExit2D) (MonoObject* obj, MonoObject* physics2DContact, MonoObject** exp);

// Created with help from this guide (Mono Embedding for Game Engines): https://nilssondev.com/mono-guide/book/

namespace Engine
{
	enum class ScriptFieldType
	{
		None = 0, Void,
		Float, Double,
		Bool, Char, String,
		SByte, Short, Int, Long,
		Byte, UShort, UInt, ULong,
		Vector2, Vector3, Vector4,
		Entity, Prefab
	};

	enum class Accessibility : uint8_t
	{
		None = 0,
		Private = (1 << 0),
		Internal = (1 << 1),
		Protected = (1 << 2),
		Public = (1 << 3)
	};

	struct ScriptMethod
	{
		ScriptFieldType ReturnType;
		uint8_t Access;

		MonoMethod* ClassMethod;

		ScriptFieldType ParamTypes[8]{};

		bool ScriptMethod::IsPublic()
		{
			return Access & (uint8_t)Accessibility::Public ? true : false;
		}
	};

	using ScriptMethodMap = std::unordered_map<std::string, ScriptMethod>;

	struct ScriptField
	{
		ScriptFieldType Type;
		uint8_t Access;

		MonoClassField* ClassField;

		bool ScriptField::IsPublic()
		{
			return Access & (uint8_t)Accessibility::Public ? true : false;
		}
	};

	// ScriptField + Data Storage
	struct ScriptFieldInstance
	{
		ScriptField Field;

		ScriptFieldInstance()
		{
			memset(m_Buffer, 0, sizeof(m_Buffer));
		}

		template<typename T>
		T GetValue()
		{
			static_assert(sizeof(T) <= 64, "Type too large!");
			return *(T*)m_Buffer;
		}

		template<typename T>
		void SetValue(T value)
		{
			static_assert(sizeof(T) <= 64, "Type too large!");
			memcpy(m_Buffer, &value, sizeof(T));
		}

		template<>
		std::string GetValue()
		{
			return m_StringBuffer;
		}

		template<>
		void SetValue(std::string value)
		{
			m_StringBuffer = value;
		}

	private:
		uint8_t m_Buffer[64];
		std::string m_StringBuffer;

		friend class ScriptEngine;
		friend class ScriptInstance;
	};

	using ScriptFieldMap = std::unordered_map<std::string, ScriptFieldInstance>;

	class ScriptClass
	{
	public:
		ScriptClass() = default;
		ScriptClass(const std::string& classNamespace, const std::string& className, bool isCore = false);
		~ScriptClass() = default;

		MonoObject* Instantiate();

		MonoClass* GetMonoClass() { return m_MonoClass; }

		MonoMethod* GetMethod(const std::string& name, int parameterCount);
		MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr);

		ScriptField GetScriptField(const std::string& fieldName) { return m_ScriptFields.at(fieldName); }
		std::unordered_map<std::string, ScriptField>& GetScriptFields() { return m_ScriptFields; }

	private:
		std::string m_ClassNamespace;
		std::string m_ClassName;
		bool m_IsCore;

		MonoClass* m_MonoClass = nullptr;

		std::unordered_map<std::string, ScriptField> m_ScriptFields;

		friend class ScriptEngine;
	};

	class ScriptEngine
	{
	public:
		ScriptEngine() = delete;

		static void Init();
		static void Shutdown();

		static void ReloadAssembly();

		static std::unordered_map<std::string, Ref<ScriptClass>> GetEntityClasses();
		static ScriptFieldMap& GetScriptFieldMap(Entity entity);
		static ScriptFieldMap GetDefaultScriptFieldMap(const std::string& scriptName);
		static ScriptMethodMap GetScriptMethodMap(const std::string& scriptName);

		static MonoAssembly* GetCoreAssembly();
		
		static Ref<ScriptClass> GetEntityClass(const std::string& name);
		static bool EntityClassExists(const std::string& className);
		static Ref<ScriptInstance> CreateEntityInstance(Entity entity, const std::string& scriptName);
		static void DeleteEntityInstance(Ref<ScriptInstance> instance, Entity entity);

		static void InstantiateAsset(AssetHandle handle);
		static void InstantiateEntity(Entity entity);
		static void OnCreateEntity(Entity entity);
		static void OnStartEntity(Entity entity);
		static void OnDestroyEntity(Entity entity);
		static void OnUpdateEntity(Entity entity, Timestep ts);
		static void OnLateUpdateEntity(Entity entity, Timestep ts);

		static void OnTriggerEnter2D(Entity entity, Physics2DContact contact2D);
		static void OnTriggerExit2D(Entity entity, Physics2DContact contact2D);
		static void OnCollisionEnter2D(Entity entity, Physics2DContact contact2D);
		static void OnCollisionExit2D(Entity entity, Physics2DContact contact2D);

		static bool EntityInstanceExists(Entity& entity);
		static Ref<ScriptInstance> GetEntityInstance(Entity entity);

		static bool AssetInstanceExists(AssetHandle handle);
		static Ref<ScriptInstance> GetAssetInstance(AssetHandle handle);

		static MonoClass* GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className);
		
		static uint8_t GetFieldAccessibility(MonoClassField* field);
		static uint8_t GetPropertyAccessbility(MonoProperty* property);
		static uint8_t GetMethodAccessbility(MonoMethod* method);

		static void HandleMonoException(MonoObject* ptrExObject);
		static MonoString* CharToMonoString(char* charString);
		static MonoString* StringToMonoString(const std::string& string);
		static std::string MonoStringToUTF8(MonoString* monoString);

		static MonoArray* CreateMonoArray(ScriptFieldType type, uint64_t count);

		template<typename T>
		static MonoArray* ArrayToMonoArray(const T* array, ScriptFieldType type, uint64_t count)
		{
			MonoArray* monoArray = CreateMonoArray(type, count);

			for (int i = 0; i < count; i++)
				mono_array_set(monoArray, T, i, array[i]);

			return monoArray;
		}
	
	private:
		static void InitMono();
		static void ShutdownMono();

		static bool LoadCoreAssembly(const std::filesystem::path& assemblyPath);
		static bool LoadAppAssembly(const std::filesystem::path& assemblyPath);

		static void LoadEntityClasses(MonoAssembly* assembly);
		static MonoObject* InstantiateClass(MonoClass* monoClass, bool isCore = false);

		friend class ScriptClass;
		friend class ScriptGlue;
	};

	class ScriptInstance
	{
	public:
		ScriptInstance() = default;
		ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity);
		ScriptInstance(Ref<ScriptClass> scriptClass, AssetHandle handle);
		~ScriptInstance() = default;

		Ref<ScriptClass> GetScriptClass() { return m_ScriptClass; }

		template<typename T>
		T GetFieldValue(const std::string& name)
		{
			static_assert(sizeof(T) <= 64, "Type too large!");

			bool success = GetFieldValueInternal(name, s_FieldValueBuffer);
			if (!success)
			{
				return T();
			}

			return *(T*)s_FieldValueBuffer;
		}

		template<>
		std::string GetFieldValue(const std::string& name)
		{
			bool success = GetFieldValueInternal(name, s_FieldValueBuffer);
			if (!success)
			{
				return {};
			}

			return ScriptEngine::MonoStringToUTF8(*(MonoString**)s_FieldValueBuffer);
		}

		template<typename T>
		void SetFieldValue(const std::string& name, T* value)
		{
			static_assert(sizeof(T) <= 64, "Type too large!");

			SetFieldValueInternal(name, value);
		}

		template<>
		void SetFieldValue(const std::string& name, std::string* value)
		{
			SetFieldValueInternal(name, ScriptEngine::StringToMonoString(*value));
		}

		void InvokeOnCreate();
		void InvokeOnStart();
		void InvokeOnDestroy();
		void InvokeOnUpdate(float ts);
		void InvokeOnLateUpdate(float ts);

		void InvokeOnTriggerEnter2D(Physics2DContact contact2D);
		void InvokeOnTriggerExit2D(Physics2DContact contact2D);
		void InvokeOnCollisionEnter2D(Physics2DContact contact2D);
		void InvokeOnCollisionExit2D(Physics2DContact contact2D);

		MonoObject* GetMonoObject() { return m_Instance; }

	private:
		bool GetFieldValueInternal(const std::string& name, void* buffer);
		bool SetFieldValueInternal(const std::string& name, const void* value);

	private:
		Ref<ScriptClass> m_ScriptClass;

		MonoObject* m_Instance = nullptr;

		MonoMethod* m_Constructor = nullptr;
		OnCreate OnCreateThunk = nullptr;
		OnStart OnStartThunk = nullptr;
		OnDestroy OnDestroyThunk = nullptr;
		OnUpdate OnUpdateThunk = nullptr;
		OnLateUpdate OnLateUpdateThunk = nullptr;

		OnTriggerEnter2D OnTriggerEnter2DThunk = nullptr;
		OnTriggerExit2D OnTriggerExit2DThunk = nullptr;
		OnCollisionEnter2D OnCollisionEnter2DThunk = nullptr;
		OnCollisionExit2D OnCollisionExit2DThunk = nullptr;

		inline static char s_FieldValueBuffer[64];
		inline static std::string s_FieldValueStringBuffer;

		friend class ScriptEngine;
		friend struct ScriptFieldInstance;
	};

	namespace Utils
	{
		inline const char* ScriptFieldTypeToString(ScriptFieldType fieldType)
		{
			switch (fieldType)
			{
				case ScriptFieldType::None:		return "None";
				case ScriptFieldType::Void:		return "Void";
				case ScriptFieldType::Float:	return "Float";
				case ScriptFieldType::Double:	return "Double";
				case ScriptFieldType::Bool:		return "Bool";
				case ScriptFieldType::Char:		return "Char";
				case ScriptFieldType::String:	return "String";
				case ScriptFieldType::SByte:	return "SByte";
				case ScriptFieldType::Short:	return "Short";
				case ScriptFieldType::Int:		return "Int";
				case ScriptFieldType::Long:		return "Long";
				case ScriptFieldType::Byte:		return "Byte";
				case ScriptFieldType::UShort:	return "UShort";
				case ScriptFieldType::UInt:		return "UInt";
				case ScriptFieldType::ULong:	return "ULong";
				case ScriptFieldType::Vector2:	return "Vector2";
				case ScriptFieldType::Vector3:	return "Vector3";
				case ScriptFieldType::Vector4:	return "Vector4";
				case ScriptFieldType::Entity:	return "Entity";
				case ScriptFieldType::Prefab:	return "Prefab";
			}

			ENGINE_CORE_ASSERT(false, "Unknown ScriptFieldType");
			return "None";
		}

		inline ScriptFieldType ScriptFieldTypeFromString(std::string_view fieldType)
		{
			if (fieldType == "None")		return ScriptFieldType::None;
			if (fieldType == "Void")		return ScriptFieldType::Void;
			if (fieldType == "Float")		return ScriptFieldType::Float;
			if (fieldType == "Double")		return ScriptFieldType::Double;
			if (fieldType == "Bool")		return ScriptFieldType::Bool;
			if (fieldType == "Char")		return ScriptFieldType::Char;
			if (fieldType == "String")		return ScriptFieldType::String;
			if (fieldType == "SByte")		return ScriptFieldType::SByte;
			if (fieldType == "Short")		return ScriptFieldType::Short;
			if (fieldType == "Int")			return ScriptFieldType::Int;
			if (fieldType == "Long")		return ScriptFieldType::Long;
			if (fieldType == "Byte")		return ScriptFieldType::Byte;
			if (fieldType == "UShort")		return ScriptFieldType::UShort;
			if (fieldType == "UInt")		return ScriptFieldType::UInt;
			if (fieldType == "ULong")		return ScriptFieldType::ULong;
			if (fieldType == "Vector2")		return ScriptFieldType::Vector2;
			if (fieldType == "Vector3")		return ScriptFieldType::Vector3;
			if (fieldType == "Vector4")		return ScriptFieldType::Vector4;
			if (fieldType == "Entity")		return ScriptFieldType::Entity;
			if (fieldType == "Prefab")		return ScriptFieldType::Prefab;

			ENGINE_CORE_ASSERT(false, "Unknown ScriptFieldType");
			return ScriptFieldType::None;
		}
	}
}

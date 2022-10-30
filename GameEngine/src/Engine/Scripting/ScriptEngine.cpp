#include "enginepch.h"
#include "Engine/Scripting/ScriptEngine.h"
#include "Engine/Scripting/ScriptGlue.h"

#include "Engine/Utils/FileUtils.h"

#include "Engine/Core/UUID.h"
#include "Engine/Scene/Entity.h"

#include <mono/jit/jit.h>
#include <mono/metadata/attrdefs.h>
#include <mono/metadata/object.h>
#include <mono/metadata/debug-helpers.h>

#include <glm/glm.hpp>

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

	namespace Utils
	{
		static ScriptFieldType MonoTypeToScriptFieldType(MonoType* monoType)
		{
			std::string typeName = mono_type_get_name(monoType);

			auto it = s_ScriptFieldTypeMap.find(typeName);
			if (it == s_ScriptFieldTypeMap.end())
			{
				ENGINE_CORE_ERROR("Unknown type: {}", typeName);
				return ScriptFieldType::None;
			}

			return it->second;
		}
	}

	struct ScriptEngineData
	{
		MonoDomain* RootDomain = nullptr;
		MonoDomain* AppDomain = nullptr;

		MonoAssembly* CoreAssembly = nullptr;
		MonoAssembly* AppAssembly = nullptr;

		MonoClass* EntityClass = nullptr;

		std::unordered_map<std::string, Ref<ScriptClass>> EntityClasses;
		std::unordered_map<UUID, Ref<ScriptInstance>> EntityInstances;
		std::unordered_map<UUID, ScriptFieldMap> EntityScriptFields;

		Scene* SceneContext;
	};

	static ScriptEngineData* s_ScriptEngineData = nullptr;


	static bool CheckMonoError(MonoError& error)
	{
		bool hasError = !mono_error_ok(&error);
		if (hasError)
		{
			unsigned short errorCode = mono_error_get_error_code(&error);
			const char* errorMessage = mono_error_get_message(&error);

			printf("Mono Error!\n");
			printf("\tError Code: %hu\n", errorCode);
			printf("\tError Message: %s\n", errorMessage);

			mono_error_cleanup(&error);
		}
		return hasError;
	}

	MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath)
	{
		uint32_t fileSize = 0;
		char* fileData = FileUtils::ReadBytes(assemblyPath, &fileSize);

		// NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
		MonoImageOpenStatus status;
		MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

		if (status != MONO_IMAGE_OK)
		{
			const char* errorMessage = mono_image_strerror(status);
			// Log some error message using the errorMessage data
			ENGINE_CORE_ERROR("Error Loading Mono Assembly: " + std::string(errorMessage));
			return nullptr;
		}

		std::string pathString = assemblyPath.string();
		MonoAssembly* assembly = mono_assembly_load_from_full(image, pathString.c_str(), &status, 0);
		mono_image_close(image);

		// Don't forget to free the file data
		delete[] fileData;

		return assembly;
	}

	void PrintAssemblyTypes(MonoAssembly* assembly)
	{
		MonoImage* image = mono_assembly_get_image(assembly);
		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

		ENGINE_CORE_TRACE("Mono Class Types:");

		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

			ENGINE_CORE_TRACE("\t{}.{}", nameSpace, name);
		}
	}

	void ScriptEngine::Init()
	{
		ENGINE_PROFILE_FUNCTION();

		s_ScriptEngineData = new ScriptEngineData();

		InitMono();

		LoadCoreAssembly("Resources/Scripts/Binaries/Engine-ScriptCore.dll");
		LoadAppAssembly("GameProject/Assets/Scripts/Binaries/GameProject.dll");
		LoadEntityClasses(s_ScriptEngineData->AppAssembly);

		InternalCalls::ScriptGlue::RegisterComponentTypes();
		InternalCalls::ScriptGlue::RegisterInternalCalls();
	}

	void ScriptEngine::Shutdown()
	{
		ENGINE_PROFILE_FUNCTION();

		ShutdownMono();

		s_ScriptEngineData->EntityInstances.clear();
		delete s_ScriptEngineData;
	}

	void ScriptEngine::InitMono()
	{
		// Mono
		mono_set_assemblies_path("../GameEngine/vendor/Mono/lib");

		s_ScriptEngineData->RootDomain = mono_jit_init("Engine-ScriptCore");
		ENGINE_CORE_ASSERT(s_ScriptEngineData->RootDomain, "Root Domain could not be initialized!");
	}

	void ScriptEngine::ShutdownMono()
	{
		s_ScriptEngineData->CoreAssembly = nullptr;
		s_ScriptEngineData->AppAssembly = nullptr;

		s_ScriptEngineData->AppDomain = nullptr;
		mono_jit_cleanup(s_ScriptEngineData->RootDomain);
	}

	void ScriptEngine::LoadCoreAssembly(const std::filesystem::path& assemblyPath)
	{
		s_ScriptEngineData->AppDomain = mono_domain_create_appdomain("EngineScriptRuntime", nullptr);
		ENGINE_CORE_ASSERT(s_ScriptEngineData->AppDomain, "App Domain could not be initialized!");
		mono_domain_set(s_ScriptEngineData->AppDomain, true);

		s_ScriptEngineData->CoreAssembly = LoadMonoAssembly(assemblyPath);
		//PrintAssemblyTypes(s_ScriptEngineData->CoreAssembly);

		MonoImage* image = mono_assembly_get_image(s_ScriptEngineData->CoreAssembly);
		s_ScriptEngineData->EntityClass = mono_class_from_name(image, "Engine.Scene", "Entity");
	}

	void ScriptEngine::LoadAppAssembly(const std::filesystem::path& assemblyPath)
	{
		s_ScriptEngineData->AppAssembly = LoadMonoAssembly(assemblyPath);
		PrintAssemblyTypes(s_ScriptEngineData->AppAssembly);
	}

	void ScriptEngine::LoadEntityClasses(MonoAssembly* assembly)
	{
		s_ScriptEngineData->EntityClasses.clear();

		MonoImage* image = mono_assembly_get_image(assembly);
		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* className = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

			std::string fullName = strlen(nameSpace) != 0 ? fmt::format("{}.{}", nameSpace, className) : className;

			MonoClass* monoClass = mono_class_from_name(image, nameSpace, className);

			if (monoClass == nullptr || monoClass == s_ScriptEngineData->EntityClass) continue; // skip when mono class is base entity class

			if (!mono_class_is_subclass_of(monoClass, s_ScriptEngineData->EntityClass, false))
				continue;

			Ref<ScriptClass> scriptClass = CreateRef<ScriptClass>(nameSpace, className);
			s_ScriptEngineData->EntityClasses[fullName] = scriptClass;

			// get class fields
			int fieldCount = mono_class_num_fields(monoClass);
			ENGINE_CORE_TRACE("{} has {} fields:", className, fieldCount);
			void* iterator = nullptr;
			while (MonoClassField* field = mono_class_get_fields(monoClass, &iterator))
			{
				const char* fieldName = mono_field_get_name(field);
				uint8_t fieldAccess = GetFieldAccessibility(field);
				ScriptFieldType fieldType = Utils::MonoTypeToScriptFieldType(mono_field_get_type(field));

				ScriptField scriptField = { fieldType, fieldAccess, field };
				if (scriptField.IsPublic()) // tracking only public fields for now, want others for debug later
				{
					scriptClass->m_ScriptFields[fieldName] = scriptField;
				}
			}
		}
	}

	void ScriptEngine::OnRuntimeStart(Scene* scene)
	{
		s_ScriptEngineData->SceneContext = scene;
	}

	void ScriptEngine::OnRuntimeStop()
	{
		s_ScriptEngineData->SceneContext = nullptr;
	}

	MonoObject* ScriptEngine::InstantiateClass(MonoClass* monoClass)
	{
		MonoObject* instance = mono_object_new(s_ScriptEngineData->AppDomain, monoClass);
		mono_runtime_object_init(instance);

		return instance;
	}

	std::unordered_map<std::string, Ref<ScriptClass>> ScriptEngine::GetEntityClasses()
	{
		return s_ScriptEngineData->EntityClasses;
	}

	Engine::ScriptFieldMap& ScriptEngine::GetScriptFieldMap(Entity entity)
	{
		ENGINE_CORE_ASSERT(entity, "Entity doesn't exists!");

		return s_ScriptEngineData->EntityScriptFields[entity.GetUUID()];
	}

	Scene* ScriptEngine::GetSceneContext()
	{
		return s_ScriptEngineData->SceneContext;
	}

	MonoAssembly* ScriptEngine::GetCoreAssembly()
	{
		return s_ScriptEngineData->CoreAssembly;
	}

	Engine::Ref<Engine::ScriptClass> ScriptEngine::GetEntityClass(const std::string& name)
	{
		if (!EntityClassExists(name))
			return nullptr;

		return s_ScriptEngineData->EntityClasses.at(name);
	}

	bool ScriptEngine::EntityClassExists(const std::string& className)
	{
		const auto& entityClasses = s_ScriptEngineData->EntityClasses;
		if (entityClasses.find(className) != entityClasses.end())
		{
			return true;
		}

		//ENGINE_CORE_ERROR("Entity Class of " + className + " could not be found!");
		return false;
	}

	Ref<ScriptInstance> ScriptEngine::CreateEntityInstance(Entity entity, const std::string& scriptName)
	{
		if (EntityClassExists(scriptName))
		{
			if (EntityInstanceExists(entity))
			{
				ENGINE_CORE_ERROR("Tried to Create Entity Instance for {}, but it already existed!", entity.GetUUID());
				return s_ScriptEngineData->EntityInstances.at(entity.GetUUID());
			}

			Ref<ScriptClass> scriptClass = s_ScriptEngineData->EntityClasses.at(scriptName);
			Ref<ScriptInstance> instance = CreateRef<ScriptInstance>(scriptClass, entity);
			s_ScriptEngineData->EntityInstances[entity.GetUUID()] = instance;

			return instance;
		}

		return nullptr;
	}

	void ScriptEngine::DeleteEntityInstance(Ref<ScriptInstance> instance, Entity entity)
	{
		if (EntityInstanceExists(entity))
		{
			s_ScriptEngineData->EntityInstances.erase(entity.GetUUID());
			instance = nullptr;
		}
	}

	void ScriptEngine::OnCreateEntity(Entity entity)
	{
		const auto& sc = entity.GetComponent<ScriptComponent>();

		if (EntityClassExists(sc.ClassName))
		{
			UUID entityID = entity.GetUUID();
			Ref<ScriptInstance> instance = CreateRef<ScriptInstance>(s_ScriptEngineData->EntityClasses.at(sc.ClassName), entity);
			ENGINE_CORE_ASSERT(instance, "Script Instance failed to be created for entityID: " + std::to_string(entityID) + ", with class: " + sc.ClassName);
			s_ScriptEngineData->EntityInstances[entityID] = instance;

			// Copy field values
			if (s_ScriptEngineData->EntityScriptFields.find(entityID) != s_ScriptEngineData->EntityScriptFields.end())
			{
				const ScriptFieldMap& fieldMap = s_ScriptEngineData->EntityScriptFields.at(entityID);
				for (const auto& [name, fieldInstance] : fieldMap)
				{
					instance->SetFieldValueInternal(name, fieldInstance.m_Buffer);
				}
			}

			instance->InvokeOnCreate();
		}
	}

	void ScriptEngine::OnDestroyEntity(Entity entity)
	{
		const auto& sc = entity.GetComponent<ScriptComponent>();

		if (EntityClassExists(sc.ClassName))
		{
			if (EntityInstanceExists(entity))
			{
				Ref<ScriptInstance> instance = s_ScriptEngineData->EntityInstances.at(entity.GetUUID());
				instance->InvokeOnDestroy();

				DeleteEntityInstance(instance, entity);
			}
		}
	}

	void ScriptEngine::OnUpdateEntity(Entity entity, Timestep ts)
	{
		const auto& sc = entity.GetComponent<ScriptComponent>();

		if (EntityClassExists(sc.ClassName))
		{
			if (EntityInstanceExists(entity))
			{
				s_ScriptEngineData->EntityInstances.at(entity.GetUUID())->InvokeOnUpdate(ts);
			}
		}
	}

	bool ScriptEngine::EntityInstanceExists(Entity& entity)
	{
		const auto& entityInstances = s_ScriptEngineData->EntityInstances;
		if (entityInstances.find(entity.GetUUID()) != entityInstances.end())
		{
			return true;
		}

		//ENGINE_CORE_ERROR("Entity Instance for {}, does not exists!", entity.GetUUID());
		return false;
	}

	Engine::Ref<Engine::ScriptInstance> ScriptEngine::GetEntityInstance(Entity entity)
	{
		if (EntityInstanceExists(entity))
		{
			return s_ScriptEngineData->EntityInstances.at(entity.GetUUID());
		}

		return nullptr;
	}

	MonoClass* ScriptEngine::GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className)
	{
		MonoImage* image = mono_assembly_get_image(assembly);
		MonoClass* klass = mono_class_from_name(image, namespaceName, className);

		if (klass == nullptr)
		{
			// Log error here
			ENGINE_CORE_ERROR("Could not find: " + std::string(namespaceName) + "." + std::string(className) + " in mono assembly image!");
			return nullptr;
		}

		return klass;
	}

	ScriptInstance::ScriptInstance(Ref<ScriptClass> scriptClass, Entity entity)
		: m_ScriptClass(scriptClass)
	{
		m_Instance = m_ScriptClass->Instantiate();

		MonoMethod* constructor = mono_class_get_method_from_name(s_ScriptEngineData->EntityClass, ".ctor", 1);
		{
			UUID id = entity.GetUUID();
			void* param = &id;
			m_ScriptClass->InvokeMethod(m_Instance, constructor, &param);
		}

		MonoClass* monoClass = m_ScriptClass->GetMonoClass();

		// setup onCreate method
		MonoMethod* OnCreateMethodPtr = mono_class_get_method_from_name(monoClass, "OnCreate", 0);
		if (OnCreateMethodPtr)
		{
			OnCreateThunk = (OnCreate)mono_method_get_unmanaged_thunk(OnCreateMethodPtr);
		}
		else
		{
			ENGINE_CORE_WARN("Could not find create method desc in class!");
		}

		// setup onDestroy method
		MonoMethod* OnDestroyMethodPtr = mono_class_get_method_from_name(monoClass, "OnDestroy", 0);
		if (OnDestroyMethodPtr)
		{
			OnDestroyThunk = (OnDestroy)mono_method_get_unmanaged_thunk(OnDestroyMethodPtr);
		}
		else
		{
			ENGINE_CORE_WARN("Could not find destroy method desc in class!");
		}

		// setup onUpdate method
		MonoMethod* OnUpdateMethodPtr = mono_class_get_method_from_name(monoClass, "OnUpdate", 1);
		if (OnUpdateMethodPtr)
		{
			OnUpdateThunk = (OnUpdate)mono_method_get_unmanaged_thunk(OnUpdateMethodPtr);
		}
		else
		{
			ENGINE_CORE_WARN("Could not find update method desc in class!");
		}
	}

	void ScriptInstance::InvokeOnCreate()
	{
		if (!OnCreateThunk) return; // handle script without OnCreate

		MonoObject* ptrExObject = nullptr;
		OnCreateThunk(m_Instance, &ptrExObject);
		ScriptEngine::HandleMonoException(ptrExObject);
	}

	void ScriptInstance::InvokeOnDestroy()
	{
		if (!OnDestroyThunk) return; // handle script without OnDestroy

		MonoObject* ptrExObject = nullptr;
		OnDestroyThunk(m_Instance, &ptrExObject);
		ScriptEngine::HandleMonoException(ptrExObject);
	}

	void ScriptInstance::InvokeOnUpdate(float ts)
	{
		if (!OnUpdateThunk) return; // handle script without OnUpdate

		MonoObject* ptrExObject = nullptr;
		OnUpdateThunk(m_Instance, &ts, &ptrExObject);
		ScriptEngine::HandleMonoException(ptrExObject);
	}

	bool ScriptInstance::GetFieldValueInternal(const std::string& name, void* buffer)
	{
		const auto& fields = m_ScriptClass->GetScriptFields();
		auto it = fields.find(name);
		if (it == fields.end())
			return false;

		const ScriptField& field = it->second;
		mono_field_get_value(m_Instance, field.ClassField, buffer);
		return true;
	}

	bool ScriptInstance::SetFieldValueInternal(const std::string& name, const void* value)
	{
		const auto& fields = m_ScriptClass->GetScriptFields();
		auto it = fields.find(name);
		if (it == fields.end())
			return false;

		const ScriptField& field = it->second;
		mono_field_set_value(m_Instance, field.ClassField, (void*)value);
		return true;
	}

	ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className)
		: m_ClassNamespace(classNamespace), m_ClassName(className)
	{
		m_MonoClass = ScriptEngine::GetClassInAssembly(s_ScriptEngineData->AppAssembly, m_ClassNamespace.c_str(), m_ClassName.c_str());
	}

	MonoObject* ScriptClass::Instantiate()
	{
		return ScriptEngine::InstantiateClass(m_MonoClass);
	}

	MonoMethod* ScriptClass::GetMethod(const std::string& name, int parameterCount)
	{
		return mono_class_get_method_from_name(m_MonoClass, name.c_str(), parameterCount);
	}

	MonoObject* ScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** params)
	{
		MonoObject* monoException = nullptr;
		MonoObject* result = mono_runtime_invoke(method, instance, params, &monoException);

		ScriptEngine::HandleMonoException(monoException);

		return result;
	}

	uint8_t ScriptEngine::GetFieldAccessibility(MonoClassField* field)
	{
		uint8_t accessibility = (uint8_t)Accessibility::None;
		uint32_t accessFlag = mono_field_get_flags(field) & MONO_FIELD_ATTR_FIELD_ACCESS_MASK;

		switch (accessFlag)
		{
		case MONO_FIELD_ATTR_PRIVATE:
		{
			accessibility = (uint8_t)Accessibility::Private;
			break;
		}
		case MONO_FIELD_ATTR_FAM_AND_ASSEM:
		{
			accessibility |= (uint8_t)Accessibility::Protected;
			accessibility |= (uint8_t)Accessibility::Internal;
			break;
		}
		case MONO_FIELD_ATTR_ASSEMBLY:
		{
			accessibility = (uint8_t)Accessibility::Internal;
			break;
		}
		case MONO_FIELD_ATTR_FAMILY:
		{
			accessibility = (uint8_t)Accessibility::Protected;
			break;
		}
		case MONO_FIELD_ATTR_FAM_OR_ASSEM:
		{
			accessibility |= (uint8_t)Accessibility::Private;
			accessibility |= (uint8_t)Accessibility::Protected;
			break;
		}
		case MONO_FIELD_ATTR_PUBLIC:
		{
			accessibility = (uint8_t)Accessibility::Public;
			break;
		}
		}

		return accessibility;
	}

	uint8_t ScriptEngine::GetPropertyAccessbility(MonoProperty* property)
	{
		uint8_t accessibility = (uint8_t)Accessibility::None;

		// Get a reference to the property's getter method
		MonoMethod* propertyGetter = mono_property_get_get_method(property);
		if (propertyGetter != nullptr)
		{
			// Extract the access flags from the getters flags
			uint32_t accessFlag = mono_method_get_flags(propertyGetter, nullptr) & MONO_METHOD_ATTR_ACCESS_MASK;

			switch (accessFlag)
			{
			case MONO_FIELD_ATTR_PRIVATE:
			{
				accessibility = (uint8_t)Accessibility::Private;
				break;
			}
			case MONO_FIELD_ATTR_FAM_AND_ASSEM:
			{
				accessibility |= (uint8_t)Accessibility::Protected;
				accessibility |= (uint8_t)Accessibility::Internal;
				break;
			}
			case MONO_FIELD_ATTR_ASSEMBLY:
			{
				accessibility = (uint8_t)Accessibility::Internal;
				break;
			}
			case MONO_FIELD_ATTR_FAMILY:
			{
				accessibility = (uint8_t)Accessibility::Protected;
				break;
			}
			case MONO_FIELD_ATTR_FAM_OR_ASSEM:
			{
				accessibility |= (uint8_t)Accessibility::Private;
				accessibility |= (uint8_t)Accessibility::Protected;
				break;
			}
			case MONO_FIELD_ATTR_PUBLIC:
			{
				accessibility = (uint8_t)Accessibility::Public;
				break;
			}
			}
		}

		// Get a reference to the property's setter method
		MonoMethod* propertySetter = mono_property_get_set_method(property);
		if (propertySetter != nullptr)
		{
			// Extract the access flags from the setters flags
			uint32_t accessFlag = mono_method_get_flags(propertySetter, nullptr) & MONO_METHOD_ATTR_ACCESS_MASK;
			if (accessFlag != MONO_FIELD_ATTR_PUBLIC)
				accessibility = (uint8_t)Accessibility::Private;
		}
		else
		{
			accessibility = (uint8_t)Accessibility::Private;
		}

		return accessibility;
	}

	void ScriptEngine::HandleMonoException(MonoObject* ptrExObject)
	{
		// Report Exception
		if (!ptrExObject) return;

		MonoString* exString = mono_object_to_string(ptrExObject, nullptr);

		MonoError error;
		char* utf8 = mono_string_to_utf8_checked(exString, &error);
		if (CheckMonoError(error))
			return;

		std::string result(utf8);
		mono_free(utf8);

		ENGINE_CORE_ERROR(result);
	}

	MonoString* ScriptEngine::StringToMonoString(std::string string)
	{
		return mono_string_new(s_ScriptEngineData->AppDomain, string.c_str());
	}

	std::string ScriptEngine::MonoStringToUTF8(MonoString* monoString)
	{
		if (monoString == nullptr || mono_string_length(monoString) == 0)
			return "";

		MonoError error;
		char* utf8 = mono_string_to_utf8_checked(monoString, &error);
		if (CheckMonoError(error))
			return "";

		std::string result(utf8);
		mono_free(utf8);

		return result;
	}
}

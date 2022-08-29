#include "enginepch.h"
#include "Engine/Scripting/ScriptEngine.h"
#include "Engine/Scripting/ScriptGlue.h"
#include "Engine/Scripting/ScriptInstance.h"

#include "Engine/Utils/FileUtils.h"

#include "Engine/Scene/Entity.h"

#include <mono/jit/jit.h>
#include <mono/metadata/attrdefs.h>

namespace Engine
{
	struct ScriptEngineData
	{
		MonoDomain* RootDomain = nullptr;
		MonoDomain* AppDomain = nullptr;

		MonoAssembly* CoreAssembly = nullptr;
		MonoAssembly* AppAssembly = nullptr;

		MonoClass* EntityClass = nullptr;

		std::unordered_map<std::string, Ref<ScriptClass>> EntityClasses;
		std::unordered_map<UUID, Ref<ScriptInstance>> EntityInstances;

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
			const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

			std::string fullName = strlen(nameSpace) != 0 ? fmt::format("{}.{}", nameSpace, name) : name;

			MonoClass* monoClass = mono_class_from_name(image, nameSpace, name);

			if (monoClass == nullptr || monoClass == s_ScriptEngineData->EntityClass) continue; // skip when mono class is base entity class

			if (mono_class_is_subclass_of(monoClass, s_ScriptEngineData->EntityClass, false))
			{
				s_ScriptEngineData->EntityClasses[fullName] = CreateRef<ScriptClass>(nameSpace, name);
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

	Scene* ScriptEngine::GetSceneContext()
	{
		return s_ScriptEngineData->SceneContext;
	}

	MonoDomain* ScriptEngine::GetRootDomain()
	{
		ENGINE_CORE_ASSERT(s_ScriptEngineData->RootDomain, "Root Domain not set.");
		return s_ScriptEngineData->RootDomain;
	}

	MonoDomain* ScriptEngine::GetAppDomain()
	{
		ENGINE_CORE_ASSERT(s_ScriptEngineData->AppDomain, "App Domain not set.");
		return s_ScriptEngineData->AppDomain;
	}

	MonoAssembly* ScriptEngine::GetCoreAssembly()
	{
		ENGINE_CORE_ASSERT(s_ScriptEngineData->CoreAssembly, "Core Assembly not set.");
		return s_ScriptEngineData->CoreAssembly;
	}

	MonoAssembly* ScriptEngine::GetAppAssembly()
	{
		ENGINE_CORE_ASSERT(s_ScriptEngineData->AppAssembly, "App Assembly not set.");
		return s_ScriptEngineData->AppAssembly;
	}

	MonoClass* ScriptEngine::GetEntityClass()
	{
		return s_ScriptEngineData->EntityClass;
	}

	bool ScriptEngine::EntityClassExists(const std::string& className)
	{
		const auto& entityClasses = s_ScriptEngineData->EntityClasses;
		if (entityClasses.find(className) != entityClasses.end())
		{
			return true;
		}

		ENGINE_CORE_WARN("Entity Class of " + className + " could not be found!");
		return false;
	}

	Ref<ScriptInstance> ScriptEngine::CreateEntityInstance(const UUID& entityID, const std::string& className)
	{
		if (EntityClassExists(className))
		{
			Ref<ScriptClass> scriptClass = s_ScriptEngineData->EntityClasses[className];

			Ref<ScriptInstance> instance = CreateRef<ScriptInstance>(scriptClass, entityID);
			s_ScriptEngineData->EntityInstances[entityID] = instance;

			return instance;
		}

		return nullptr;
	}

	void ScriptEngine::DeleteEntityInstance(Ref<ScriptInstance> instance, UUID entityID)
	{
		if (EntityInstanceExists(entityID))
		{
			s_ScriptEngineData->EntityInstances.erase(entityID);
			instance = nullptr;
		}
	}

	void ScriptEngine::OnCreateEntity(Entity entity, const std::string& className)
	{
		if (EntityClassExists(className))
		{
			UUID entityID = entity.GetUUID();
			if (EntityInstanceExists(entityID))
			{
				s_ScriptEngineData->EntityInstances[entityID]->InvokeOnCreate();
			}
			else
			{
				CreateEntityInstance(entityID, className);
			}
		}
	}

	void ScriptEngine::OnDestroyEntity(Entity entity, const std::string& className)
	{
		if (EntityClassExists(className))
		{
			UUID entityID = entity.GetUUID();
			if (EntityInstanceExists(entityID))
			{
				Ref<ScriptInstance> instance = s_ScriptEngineData->EntityInstances[entityID];
				instance->InvokeOnDestroy();

				//DeleteEntityInstance(instance, entityID);
			}
		}
	}

	void ScriptEngine::OnUpdateEntity(Entity entity, const std::string& className, Timestep ts)
	{
		if (EntityClassExists(className))
		{
			UUID entityID = entity.GetUUID();
			if (EntityInstanceExists(entityID))
			{
				s_ScriptEngineData->EntityInstances[entityID]->InvokeOnUpdate(ts);
			}
			else
			{
				CreateEntityInstance(entityID, className);
			}
		}
	}

	bool ScriptEngine::EntityInstanceExists(const UUID& entityID)
	{
		const auto& entityInstances = s_ScriptEngineData->EntityInstances;
		if (entityInstances.find(entityID) != entityInstances.end())
		{
			return true;
		}

		return false;
	}

	Engine::Ref<Engine::ScriptInstance> ScriptEngine::GetEntityInstance(const UUID& entityID)
	{
		if (EntityInstanceExists(entityID))
		{
			return s_ScriptEngineData->EntityInstances.at(entityID);
		}

		ENGINE_CORE_WARN("Entity Instances with UUID of " + std::to_string(entityID) + " could not be found!");

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

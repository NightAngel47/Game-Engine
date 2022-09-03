#include "enginepch.h"
#include "Engine/Scripting/ScriptInstance.h"
#include "Engine/Scripting/ScriptEngine.h"
#include "Engine/Scripting/ScriptField.h"
#include "Engine/Scripting/ScriptFieldData.h"

#include <mono/metadata/object.h>

namespace Engine
{
	ScriptInstance::ScriptInstance(Ref<ScriptClass> scriptClass, const UUID& entityID, const ScriptComponent* sc)
		: m_ScriptClass(scriptClass)
	{
		m_Instance = m_ScriptClass->Instantiate();

		MonoMethod* constructor = mono_class_get_method_from_name(ScriptEngine::GetEntityClass(), ".ctor", 1);
		{
			UUID id = entityID;
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

		SetInstanceFields(sc);
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

	void ScriptInstance::SetInstanceFields(const ScriptComponent* sc)
	{
		if (!sc) return;

		auto& scriptFields = m_ScriptClass->GetScriptFields();
		for (auto const& [key, val] : scriptFields)
		{
			if (val->IsPublic())
			{
				const char* typeName = val->GetTypeName();

				switch (val->GetType())
				{
				default:
					ENGINE_CORE_ERROR("Script Field Type {} not supported in Draw Component!", typeName);
					break;
				case ScriptFieldType::None:
					ENGINE_CORE_ERROR("Script Field Type {} not supported in Draw Component!", typeName);
					break;
				case ScriptFieldType::Float:
				{
					float fieldValue = sc->ScriptFieldsData.at(key)->get<float>();
					val->SetValue(m_Instance, &fieldValue);
					break;
				}
				case ScriptFieldType::Double:
					ENGINE_CORE_ERROR("Script Field Type {} not supported in Draw Component!", typeName);
					break;
				case ScriptFieldType::Bool:
					ENGINE_CORE_ERROR("Script Field Type {} not supported in Draw Component!", typeName);
					break;
				case ScriptFieldType::Char:
					ENGINE_CORE_ERROR("Script Field Type {} not supported in Draw Component!", typeName);
					break;
				case ScriptFieldType::String:
				{
					char buffer[256];
					memset(buffer, 0, sizeof(buffer));
					strcpy_s(buffer, sizeof(buffer), sc->ScriptFieldsData.at(key)->get<std::string>().c_str());
					val->SetValue(m_Instance, ScriptEngine::StringToMonoString(std::string(buffer)));
					break;
				}
				case ScriptFieldType::Byte:
					ENGINE_CORE_ERROR("Script Field Type {} not supported in Draw Component!", typeName);
					break;
				case ScriptFieldType::Short:
					ENGINE_CORE_ERROR("Script Field Type {} not supported in Draw Component!", typeName);
					break;
				case ScriptFieldType::Int:
				{
					int fieldValue = sc->ScriptFieldsData.at(key)->get<int>();
					val->SetValue(m_Instance, &fieldValue);
					break;
				}
				case ScriptFieldType::Long:
					ENGINE_CORE_ERROR("Script Field Type {} not supported in Draw Component!", typeName);
					break;
				case ScriptFieldType::UByte:
					ENGINE_CORE_ERROR("Script Field Type {} not supported in Draw Component!", typeName);
					break;
				case ScriptFieldType::UShort:
					ENGINE_CORE_ERROR("Script Field Type {} not supported in Draw Component!", typeName);
					break;
				case ScriptFieldType::UInt:
					ENGINE_CORE_ERROR("Script Field Type {} not supported in Draw Component!", typeName);
					break;
				case ScriptFieldType::ULong:
					ENGINE_CORE_ERROR("Script Field Type {} not supported in Draw Component!", typeName);
					break;
				case ScriptFieldType::Vector2:
				{
					glm::vec2 fieldValue = sc->ScriptFieldsData.at(key)->get<glm::vec2>();
					val->SetValue(m_Instance, &fieldValue);
					break;
				}
				case ScriptFieldType::Vector3:
				{
					glm::vec3 fieldValue = sc->ScriptFieldsData.at(key)->get<glm::vec3>();
					val->SetValue(m_Instance, &fieldValue);
					break;
				}
					break;
				case ScriptFieldType::Vector4:
				{
					glm::vec3 fieldValue = sc->ScriptFieldsData.at(key)->get<glm::vec3>();
					val->SetValue(m_Instance, &fieldValue);
					break;
				}
					break;
				case ScriptFieldType::Entity:
					ENGINE_CORE_ERROR("Script Field Type {} not supported in Draw Component!", typeName);
					break;
				}
			}
		}
	}

}

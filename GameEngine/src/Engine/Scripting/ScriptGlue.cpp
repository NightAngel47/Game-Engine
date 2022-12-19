#include "enginepch.h"
#include "Engine/Scripting/ScriptGlue.h"
#include "Engine/Scripting/ScriptEngine.h"

#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Components.h"
#include "Engine/Math/Random.h"

namespace InternalCalls
{
	static std::unordered_map < MonoType*, std::function<bool(Engine::Entity) >> s_EntityHasComponentFuncs;

	static Engine::Entity GetEntityFromScene(Engine::UUID entityID)
	{
		Engine::Scene* scene = Engine::ScriptEngine::GetSceneContext();
		ENGINE_CORE_ASSERT(scene, "Active Scene Context was not set in Script Engine!");
		Engine::Entity entity = scene->GetEntityWithUUID(entityID);
		ENGINE_CORE_ASSERT(entity, "Entity with UUID: " + std::to_string(entityID) + " was not found in Scene!");
		return entity;
	}

	static Engine::Entity GetEntityFromScene(std::string entityName)
	{
		Engine::Scene* scene = Engine::ScriptEngine::GetSceneContext();
		ENGINE_CORE_ASSERT(scene, "Active Scene Context was not set in Script Engine!");
		Engine::Entity entity = scene->FindEntityByName(entityName);
		ENGINE_CORE_ASSERT(entity, "Entity with name: " + entityName + " was not found in Scene!");
		return entity;
	}

#define ENGINE_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Engine.Core.InternalCalls::" #Name, Name)

	void ScriptGlue::RegisterInternalCalls()
	{
		ENGINE_PROFILE_FUNCTION();

		// Add internal calls

		ENGINE_ADD_INTERNAL_CALL(Log_Trace);
		ENGINE_ADD_INTERNAL_CALL(Log_Info);
		ENGINE_ADD_INTERNAL_CALL(Log_Warn);
		ENGINE_ADD_INTERNAL_CALL(Log_Error);
		ENGINE_ADD_INTERNAL_CALL(Log_Critical);

		ENGINE_ADD_INTERNAL_CALL(Input_IsKeyPressed);
		ENGINE_ADD_INTERNAL_CALL(Input_IsMouseButtonPressed);
		ENGINE_ADD_INTERNAL_CALL(Input_GetMousePosition);
		ENGINE_ADD_INTERNAL_CALL(Input_GetMouseY);
		ENGINE_ADD_INTERNAL_CALL(Input_GetMouseX);

		ENGINE_ADD_INTERNAL_CALL(Random_Float);
		ENGINE_ADD_INTERNAL_CALL(Random_Float_Seed);
		ENGINE_ADD_INTERNAL_CALL(Random_Int);
		ENGINE_ADD_INTERNAL_CALL(Random_Int_Seed);
		ENGINE_ADD_INTERNAL_CALL(Random_Range_Float);
		ENGINE_ADD_INTERNAL_CALL(Random_Range_Float_Seed);
		ENGINE_ADD_INTERNAL_CALL(Random_Range_Int);
		ENGINE_ADD_INTERNAL_CALL(Random_Range_Int_Seed);

		ENGINE_ADD_INTERNAL_CALL(Vector2_Magnitude);
		ENGINE_ADD_INTERNAL_CALL(Vector2_sqrMagnitude);

		ENGINE_ADD_INTERNAL_CALL(Vector3_Magnitude);
		ENGINE_ADD_INTERNAL_CALL(Vector3_sqrMagnitude);

		ENGINE_ADD_INTERNAL_CALL(Vector4_Magnitude);
		ENGINE_ADD_INTERNAL_CALL(Vector4_sqrMagnitude);

		ENGINE_ADD_INTERNAL_CALL(Entity_HasComponent);
		ENGINE_ADD_INTERNAL_CALL(Entity_FindEntityByName);
		ENGINE_ADD_INTERNAL_CALL(Entity_GetScriptInstance);

		ENGINE_ADD_INTERNAL_CALL(TransformComponent_GetPosition);
		ENGINE_ADD_INTERNAL_CALL(TransformComponent_SetPosition);
		ENGINE_ADD_INTERNAL_CALL(TransformComponent_GetRotation);
		ENGINE_ADD_INTERNAL_CALL(TransformComponent_SetRotation);
		ENGINE_ADD_INTERNAL_CALL(TransformComponent_GetScale);
		ENGINE_ADD_INTERNAL_CALL(TransformComponent_SetScale);

		ENGINE_ADD_INTERNAL_CALL(SpriteRendererComponent_GetColor);
		ENGINE_ADD_INTERNAL_CALL(SpriteRendererComponent_SetColor);
		ENGINE_ADD_INTERNAL_CALL(SpriteRendererComponent_GetTiling);
		ENGINE_ADD_INTERNAL_CALL(SpriteRendererComponent_SetTiling);
	}

	template<typename... Component>
	static void RegisterComponent()
	{
		([]()
		{
			std::string_view typeName = typeid(Component).name();
			size_t pos = typeName.find_last_of(':');
			std::string_view structName = typeName.substr(pos + 1);
			std::string managedTypeName = fmt::format("Engine.Scene.{}", structName);

			MonoType* managedType = mono_reflection_type_from_name(managedTypeName.data(), mono_assembly_get_image(Engine::ScriptEngine::GetCoreAssembly()));
			if (!managedType)
			{
				ENGINE_CORE_WARN("Could not register component: {} for scripting!", managedTypeName);
				return;
			}
			s_EntityHasComponentFuncs[managedType] = [](Engine::Entity entity) { return entity.HasComponent<Component>(); };
		}(), ... );
	}

	template<typename... Component>
	static void RegisterComponent(Engine::ComponentGroup<Component...>)
	{
		RegisterComponent<Component...>();
	}

	void ScriptGlue::RegisterComponentTypes()
	{
		RegisterComponent(Engine::AllComponents{});
	}

#pragma region Log

	void ScriptGlue::Log_Trace(MonoString* message)
	{
		ENGINE_TRACE(Engine::ScriptEngine::MonoStringToUTF8(message));
	}

	void ScriptGlue::Log_Info(MonoString* message)
	{
		ENGINE_INFO(Engine::ScriptEngine::MonoStringToUTF8(message));
	}

	void ScriptGlue::Log_Warn(MonoString* message)
	{
		ENGINE_WARN(Engine::ScriptEngine::MonoStringToUTF8(message));
	}

	void ScriptGlue::Log_Error(MonoString* message)
	{
		ENGINE_ERROR(Engine::ScriptEngine::MonoStringToUTF8(message));
	}

	void ScriptGlue::Log_Critical(MonoString* message)
	{
		ENGINE_CRITICAL(Engine::ScriptEngine::MonoStringToUTF8(message));
	}

#pragma endregion Log

#pragma region Input

	bool ScriptGlue::Input_IsKeyPressed(Engine::KeyCode key)
	{
		return Engine::Input::IsKeyPressed(key);
	}

	bool ScriptGlue::Input_IsMouseButtonPressed(Engine::MouseCode key)
	{
		return Engine::Input::IsMouseButtonPressed(key);
	}
	
	void ScriptGlue::Input_GetMousePosition(glm::vec2* mousePos)
	{
		*mousePos = Engine::Input::GetMousePosition();
	}

	float ScriptGlue::Input_GetMouseY()
	{
		return Engine::Input::GetMouseY();
	}

	float ScriptGlue::Input_GetMouseX()
	{
		return Engine::Input::GetMouseX();
	}

#pragma endregion Input

#pragma region Random

	float ScriptGlue::Random_Float()
	{
		return Engine::Math::Random::Float();
	}

	float ScriptGlue::Random_Float_Seed(unsigned int seed)
	{
		return Engine::Math::Random::Float(seed);
	}

	int ScriptGlue::Random_Int()
	{
		return Engine::Math::Random::Int();
	}

	int ScriptGlue::Random_Int_Seed(unsigned int seed)
	{
		return Engine::Math::Random::Int(seed);
	}

	float ScriptGlue::Random_Range_Float(float min, float max)
	{
		return Engine::Math::Random::Range(min, max);
	}

	float ScriptGlue::Random_Range_Float_Seed(float min, float max, unsigned int seed)
	{
		return Engine::Math::Random::Range(min, max, seed);
	}

	int ScriptGlue::Random_Range_Int(int min, int max)
	{
		return Engine::Math::Random::Range(min, max);
	}

	int ScriptGlue::Random_Range_Int_Seed(int min, int max, unsigned int seed)
	{
		return Engine::Math::Random::Range(min, max, seed);
	}

#pragma endregion Random

#pragma region Vector2

	float ScriptGlue::Vector2_Magnitude(glm::vec2& vector2)
	{
		return glm::length(vector2);
	}

	float ScriptGlue::Vector2_sqrMagnitude(glm::vec2& vector2)
	{
		return glm::dot(vector2, vector2);
	}

#pragma endregion Vector2
	
#pragma region Vector3

	float ScriptGlue::Vector3_Magnitude(glm::vec3& vector3)
	{
		return glm::length(vector3);
	}

	float ScriptGlue::Vector3_sqrMagnitude(glm::vec3& vector3)
	{
		return glm::dot(vector3, vector3);
	}

#pragma endregion Vector3

#pragma region Vector4

	float ScriptGlue::Vector4_Magnitude(glm::vec4& vector4)
	{
		return glm::length(vector4);
	}

	float ScriptGlue::Vector4_sqrMagnitude(glm::vec4& vector4)
	{
		return glm::dot(vector4, vector4);
	}

#pragma endregion Vector4

#pragma region Entity

	bool ScriptGlue::Entity_HasComponent(Engine::UUID entityID, MonoReflectionType* componentType)
	{
		Engine::Entity entity = GetEntityFromScene(entityID);
		MonoType* managedType = mono_reflection_type_get_type(componentType);
		ENGINE_CORE_ASSERT(s_EntityHasComponentFuncs.find(managedType) != s_EntityHasComponentFuncs.end());
		return s_EntityHasComponentFuncs.at(managedType)(entity);
	}

	uint64_t ScriptGlue::Entity_FindEntityByName(MonoString* name)
	{
		std::string entityName = Engine::ScriptEngine::MonoStringToUTF8(name);
		Engine::Entity entity = GetEntityFromScene(entityName);

		if (!entity)
		{
			return 0;
		}

		return entity.GetUUID();
	}

	MonoObject* ScriptGlue::Entity_GetScriptInstance(Engine::UUID entityID)
	{
		Engine::Entity entity = GetEntityFromScene(entityID);

		auto& instance = Engine::ScriptEngine::GetEntityInstance(entity);

		return instance->GetMonoObject();
	}

#pragma endregion Entity

#pragma region TransformComponent

	void ScriptGlue::TransformComponent_GetPosition(Engine::UUID entityID, glm::vec3* position)
	{
		Engine::Entity entity = GetEntityFromScene(entityID);
		*position = entity.GetComponent<Engine::TransformComponent>().Position;
	}

	void ScriptGlue::TransformComponent_SetPosition(Engine::UUID entityID, glm::vec3& position)
	{
		Engine::Entity entity = GetEntityFromScene(entityID);
		entity.GetComponent<Engine::TransformComponent>().Position = position;
	}

	void ScriptGlue::TransformComponent_GetRotation(Engine::UUID entityID, glm::vec3* rotation)
	{
		Engine::Entity entity = GetEntityFromScene(entityID);
		*rotation = entity.GetComponent<Engine::TransformComponent>().Rotation;
	}

	void ScriptGlue::TransformComponent_SetRotation(Engine::UUID entityID, glm::vec3& rotation)
	{
		Engine::Entity entity = GetEntityFromScene(entityID);
		entity.GetComponent<Engine::TransformComponent>().Rotation = rotation;
	}

	void ScriptGlue::TransformComponent_GetScale(Engine::UUID entityID, glm::vec3* scale)
	{
		Engine::Entity entity = GetEntityFromScene(entityID);
		*scale = entity.GetComponent<Engine::TransformComponent>().Scale;
	}

	void ScriptGlue::TransformComponent_SetScale(Engine::UUID entityID, glm::vec3& scale)
	{
		Engine::Entity entity = GetEntityFromScene(entityID);
		entity.GetComponent<Engine::TransformComponent>().Scale = scale;
	}

#pragma endregion TransformComponent

#pragma region SpriteRendererComponent

	void ScriptGlue::SpriteRendererComponent_GetColor(Engine::UUID entityID, glm::vec4* color)
	{
		Engine::Entity entity = GetEntityFromScene(entityID);
		*color = entity.GetComponent<Engine::SpriteRendererComponent>().Color;
	}

	void ScriptGlue::SpriteRendererComponent_SetColor(Engine::UUID entityID, glm::vec4& color)
	{
		Engine::Entity entity = GetEntityFromScene(entityID);
		entity.GetComponent<Engine::SpriteRendererComponent>().Color = color;
	}

	float ScriptGlue::SpriteRendererComponent_GetTiling(Engine::UUID entityID)
	{
		Engine::Entity entity = GetEntityFromScene(entityID);
		return entity.GetComponent<Engine::SpriteRendererComponent>().Tiling;
	}

	void ScriptGlue::SpriteRendererComponent_SetTiling(Engine::UUID entityID, float tiling)
	{
		Engine::Entity entity = GetEntityFromScene(entityID);
		entity.GetComponent<Engine::SpriteRendererComponent>().Tiling = tiling;
	}

#pragma endregion SpriteRendererComponent
}

#include "enginepch.h"
#include "Engine/Scripting/ScriptGlue.h"
#include <Engine/Scripting/ScriptEngine.h>

#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Components.h"

namespace InternalCalls
{

#define ENGINE_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Engine.Core.InternalCalls::" #Name, Name)

	void ScriptGlue::RegisterInternalCalls()
	{
		ENGINE_PROFILE_FUNCTION();

		// Add internal calls

#pragma region Log

		ENGINE_ADD_INTERNAL_CALL(Log_Trace);
		ENGINE_ADD_INTERNAL_CALL(Log_Info);
		ENGINE_ADD_INTERNAL_CALL(Log_Warn);
		ENGINE_ADD_INTERNAL_CALL(Log_Error);
		ENGINE_ADD_INTERNAL_CALL(Log_Critical);

#pragma endregion

#pragma region Input

		ENGINE_ADD_INTERNAL_CALL(Input_IsKeyPressed);
		ENGINE_ADD_INTERNAL_CALL(Input_IsMouseButtonPressed);
		ENGINE_ADD_INTERNAL_CALL(Input_GetMousePosition);
		ENGINE_ADD_INTERNAL_CALL(Input_GetMouseY);
		ENGINE_ADD_INTERNAL_CALL(Input_GetMouseX);

#pragma endregion

#pragma region Entity

		ENGINE_ADD_INTERNAL_CALL(Entity_GetComponent_Tag);
		ENGINE_ADD_INTERNAL_CALL(Entity_GetComponent_Transform);

#pragma endregion

#pragma region Transform Component

		ENGINE_ADD_INTERNAL_CALL(TransformComponent_SetPosition);
		ENGINE_ADD_INTERNAL_CALL(TransformComponent_SetRotation);
		ENGINE_ADD_INTERNAL_CALL(TransformComponent_SetScale);

#pragma endregion

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

#pragma endregion

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

#pragma endregion

#pragma region Entity

	void ScriptGlue::Entity_GetComponent_Tag(Engine::UUID entityID, TagData* outTag)
	{
		Engine::Entity entity = Engine::ScriptEngine::GetSceneContext()->GetEntityWithUUID(entityID);
		auto& tag = entity.GetComponent<Engine::TagComponent>();

		outTag->tag = mono_string_new(Engine::ScriptEngine::GetAppDomain(), tag.Tag.c_str());
	}

	void ScriptGlue::Entity_GetComponent_Transform(Engine::UUID entityID, TransformData* outTransform)
	{
		Engine::Entity entity = Engine::ScriptEngine::GetSceneContext()->GetEntityWithUUID(entityID);
		auto& tc = entity.GetComponent<Engine::TransformComponent>();

		outTransform->position = tc.Position;
		outTransform->rotation = tc.Rotation;
		outTransform->scale = tc.Scale;
	}

#pragma endregion

#pragma region Transform Component

	void ScriptGlue::TransformComponent_SetPosition(Engine::UUID entityID, glm::vec3& position)
	{
		Engine::Entity entity = Engine::ScriptEngine::GetSceneContext()->GetEntityWithUUID(entityID);
		auto& tc = entity.GetComponent<Engine::TransformComponent>();

		tc.Position = position;
	}

	void ScriptGlue::TransformComponent_SetRotation(Engine::UUID entityID, glm::vec3& rotation)
	{
		Engine::Entity entity = Engine::ScriptEngine::GetSceneContext()->GetEntityWithUUID(entityID);
		auto& tc = entity.GetComponent<Engine::TransformComponent>();

		tc.Rotation = rotation;
	}

	void ScriptGlue::TransformComponent_SetScale(Engine::UUID entityID, glm::vec3& scale)
	{
		Engine::Entity entity = Engine::ScriptEngine::GetSceneContext()->GetEntityWithUUID(entityID);
		auto& tc = entity.GetComponent<Engine::TransformComponent>();

		tc.Scale = scale;
	}

#pragma endregion
}

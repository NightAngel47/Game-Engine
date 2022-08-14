#include "enginepch.h"
#include "Engine/Scripting/ScriptGlue.h"
#include <Engine/Scripting/ScriptEngine.h>

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
		auto view = Engine::ScriptEngine::GetSceneContext()->GetAllEntitiesWith<Engine::IDComponent, Engine::TagComponent>();

		for (auto entity : view)
		{
			auto [uuid, tag] = view.get<Engine::IDComponent, Engine::TagComponent>(entity);
			if (uuid.ID == entityID)
			{
				outTag->tag = mono_string_new(Engine::ScriptEngine::GetAppDomain(), tag.Tag.c_str());

				return;
			}
		}
	}

	void ScriptGlue::Entity_GetComponent_Transform(Engine::UUID entityID, TransformData* outTransform)
	{
		auto view = Engine::ScriptEngine::GetSceneContext()->GetAllEntitiesWith<Engine::IDComponent, Engine::TransformComponent>();

		for (auto entity : view)
		{
			auto [uuid, tc] = view.get<Engine::IDComponent, Engine::TransformComponent>(entity);
			if (uuid.ID == entityID)
			{
				outTransform->position = tc.Position;
				outTransform->rotation = tc.Rotation;
				outTransform->scale = tc.Scale;

				return;
			}
		}
	}

#pragma endregion

#pragma region Transform Component

	void ScriptGlue::TransformComponent_SetPosition(Engine::UUID entityID, glm::vec3& position)
	{
		auto view = Engine::ScriptEngine::GetSceneContext()->GetAllEntitiesWith<Engine::IDComponent, Engine::TransformComponent>();

		for (auto entity : view)
		{
			auto [uuid, tc] = view.get<Engine::IDComponent, Engine::TransformComponent>(entity);
			if (uuid.ID == entityID)
			{
				tc.Position = position;

				return;
			}
		}
	}

	void ScriptGlue::TransformComponent_SetRotation(Engine::UUID entityID, glm::vec3& rotation)
	{
		auto view = Engine::ScriptEngine::GetSceneContext()->GetAllEntitiesWith<Engine::IDComponent, Engine::TransformComponent>();

		for (auto entity : view)
		{
			auto [uuid, tc] = view.get<Engine::IDComponent, Engine::TransformComponent>(entity);
			if (uuid.ID == entityID)
			{
				tc.Rotation = rotation;

				return;
			}
		}
	}

	void ScriptGlue::TransformComponent_SetScale(Engine::UUID entityID, glm::vec3& scale)
	{
		auto view = Engine::ScriptEngine::GetSceneContext()->GetAllEntitiesWith<Engine::IDComponent, Engine::TransformComponent>();

		for (auto entity : view)
		{
			auto [uuid, tc] = view.get<Engine::IDComponent, Engine::TransformComponent>(entity);
			if (uuid.ID == entityID)
			{
				tc.Scale = scale;

				return;
			}
		}
	}

#pragma endregion
}

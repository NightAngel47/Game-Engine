#pragma once
#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>
#include <mono/metadata/appdomain.h>
#include <mono/metadata/debug-helpers.h>

#include "Engine/Scene/Scene.h"
#include "Engine/Core/KeyCodes.h"
#include "Engine/Core/MouseCodes.h"

#include <glm/glm.hpp>

namespace InternalCalls
{
	class ScriptGlue
	{
	public:
		static void RegisterInternalCalls();
	private:
#pragma region Log

		static void Log_Trace(MonoString* message);
		static void Log_Info(MonoString* message);
		static void Log_Warn(MonoString* message);
		static void Log_Error(MonoString* message);
		static void Log_Critical(MonoString* message);

#pragma endregion

#pragma region Input

		static bool Input_IsKeyPressed(Engine::KeyCode key);
		static bool Input_IsMouseButtonPressed(Engine::MouseCode key);
		static void Input_GetMousePosition(glm::vec2* mousePos);
		static float Input_GetMouseY();
		static float Input_GetMouseX();

#pragma endregion

#pragma region Components

		struct TagData
		{
			MonoString* tag;
		};

		struct TransformData
		{
			glm::vec3 position;
			glm::vec3 rotation;
			glm::vec3 scale;
		};

#pragma endregion

#pragma region Entity

		static void Entity_GetComponent_Tag(Engine::UUID entityID, TagData* outTag);
		static void Entity_GetComponent_Transform(Engine::UUID entityID, TransformData* outTransform);

#pragma endregion

#pragma region Transform Component

		static void TransformComponent_SetPosition(Engine::UUID entityID, glm::vec3& position);
		static void TransformComponent_SetRotation(Engine::UUID entityID, glm::vec3& rotation);
		static void TransformComponent_SetScale(Engine::UUID entityID, glm::vec3& scale);

#pragma endregion
	};
}


#pragma once
#include "Engine/Core/UUID.h"
#include "Engine/Core/KeyCodes.h"
#include "Engine/Core/MouseCodes.h"

#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>
#include <mono/metadata/appdomain.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/reflection.h>

#include <glm/glm.hpp>

namespace InternalCalls
{
	class ScriptGlue
	{
	public:
		static void RegisterInternalCalls();
		static void RegisterComponentTypes();

	private:
#pragma region Log

		static void Log_Trace(MonoString* message);
		static void Log_Info(MonoString* message);
		static void Log_Warn(MonoString* message);
		static void Log_Error(MonoString* message);
		static void Log_Critical(MonoString* message);

#pragma endregion Log

#pragma region Input

		static bool Input_IsKeyPressed(Engine::KeyCode key);
		static bool Input_IsMouseButtonPressed(Engine::MouseCode key);

		static void Input_GetMousePosition(glm::vec2* mousePos);

		static float Input_GetMouseY();
		static float Input_GetMouseX();

#pragma endregion Input

#pragma region Random

		static float Random_Float();
		static float Random_Float_Seed(unsigned int seed);

		static int Random_Int();
		static int Random_Int_Seed(unsigned int seed);

		static float Random_Range_Float(float min, float max);
		static float Random_Range_Float_Seed(float min, float max, unsigned int seed);
		static int Random_Range_Int(int min, int max);
		static int Random_Range_Int_Seed(int min, int max, unsigned int seed);

#pragma endregion Random

#pragma region Entity

		static bool Entity_HasComponent(Engine::UUID entityID, MonoReflectionType* componentType);

#pragma endregion Entity

#pragma region TransformComponent

		static void TransformComponent_GetPosition(Engine::UUID entityID, glm::vec3* position);
		static void TransformComponent_SetPosition(Engine::UUID entityID, glm::vec3& position);

		static void TransformComponent_GetRotation(Engine::UUID entityID, glm::vec3* rotation);
		static void TransformComponent_SetRotation(Engine::UUID entityID, glm::vec3& rotation);

		static void TransformComponent_GetScale(Engine::UUID entityID, glm::vec3* scale);
		static void TransformComponent_SetScale(Engine::UUID entityID, glm::vec3& scale);

#pragma endregion TransformComponent

#pragma region SpriteRendererComponent

		static void SpriteRendererComponent_GetColor(Engine::UUID entityID, glm::vec4* color);
		static void SpriteRendererComponent_SetColor(Engine::UUID entityID, glm::vec4& color);

		static float SpriteRendererComponent_GetTiling(Engine::UUID entityID);
		static void SpriteRendererComponent_SetTiling(Engine::UUID entityID, float tiling);

#pragma endregion SpriteRendererComponent
	};
}


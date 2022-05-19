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
		static void InitRuntime(Engine::Ref<Engine::Scene> activeScene);
		static void ShutdownRuntime();
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
		static void Input_GetMousePosition(float& x, float& y);
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
			//float position[3];
			//float rotation[3];
			//float scale[3];

			float posX, posY, posZ;
			float rotX, rotY, rotZ;
			float scaleX, scaleY, scaleZ;
		};

#pragma endregion

#pragma region Entity

		static void Entity_GetComponent_Tag(uint64_t entityID, TagData* data);
		static void Entity_GetComponent_Transform(uint64_t entityID, TransformData* data);

#pragma endregion

#pragma region Transform Component

		static void TransformComponent_SetPosition(uint64_t entityID, float& x, float& y, float& z);

#pragma endregion
	};
}


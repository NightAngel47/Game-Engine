#include "enginepch.h"
#include "Engine/Scripting/ScriptGlue.h"

namespace InternalCalls
{
	void ScriptGlue::RegisterInternalCalls()
	{
		ENGINE_PROFILE_FUNCTION();

		// Add internal calls

#pragma region Log

		mono_add_internal_call("Engine.Core.InternalCalls::Log_Trace(string)", &Log_Trace);
		mono_add_internal_call("Engine.Core.InternalCalls::Log_Info(string)", &Log_Info);
		mono_add_internal_call("Engine.Core.InternalCalls::Log_Warn(string)", &Log_Warn);
		mono_add_internal_call("Engine.Core.InternalCalls::Log_Error(string)", &Log_Error);
		mono_add_internal_call("Engine.Core.InternalCalls::Log_Critical(string)", &Log_Critical);

#pragma endregion

#pragma region Input

		mono_add_internal_call("Engine.Core.InternalCalls::Input_IsKeyPressed(int)", &Input_IsKeyPressed);
		mono_add_internal_call("Engine.Core.InternalCalls::Input_IsMouseButtonPressed(int)", &Input_IsMouseButtonPressed);
		mono_add_internal_call("Engine.Core.InternalCalls::Input_GetMousePosition(single&,single&)", &Input_GetMousePosition);
		mono_add_internal_call("Engine.Core.InternalCalls::Input_GetMouseY()", &Input_GetMouseY);
		mono_add_internal_call("Engine.Core.InternalCalls::Input_GetMouseX()", &Input_GetMouseX);

#pragma endregion

	}

#pragma region Log

	void ScriptGlue::Log_Trace(MonoString* message)
	{
		ENGINE_TRACE(mono_string_to_utf8(message));
	}

	void ScriptGlue::Log_Info(MonoString* message)
	{
		ENGINE_INFO(mono_string_to_utf8(message));
	}

	void ScriptGlue::Log_Warn(MonoString* message)
	{
		ENGINE_WARN(mono_string_to_utf8(message));
	}

	void ScriptGlue::Log_Error(MonoString* message)
	{
		ENGINE_ERROR(mono_string_to_utf8(message));
	}

	void ScriptGlue::Log_Critical(MonoString* message)
	{
		ENGINE_CRITICAL(mono_string_to_utf8(message));
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
	
	void ScriptGlue::Input_GetMousePosition(float& x, float& y)
	{
		glm::vec2 mousePos = Engine::Input::GetMousePosition();
		
		x = mousePos.x;
		y = mousePos.y;
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
}

#include "enginepch.h"
#include "Engine/Scripting/ScriptGlue.h"

namespace InternalCalls
{
	void ScriptGlue::RegisterInternalCalls()
	{
		ENGINE_PROFILE_FUNCTION();

		// Add internal calls
		mono_add_internal_call("Engine.Source.Core.Log::Log_Trace(string)", &Log_Trace);
		mono_add_internal_call("Engine.Source.Core.Log::Log_Info(string)", &Log_Info);
		mono_add_internal_call("Engine.Source.Core.Log::Log_Warn(string)", &Log_Warn);
		mono_add_internal_call("Engine.Source.Core.Log::Log_Error(string)", &Log_Error);
		mono_add_internal_call("Engine.Source.Core.Log::Log_Critical(string)", &Log_Critical);
	}

#pragma region Log

	void ScriptGlue::Log_Trace(MonoString* message)
	{
		std::string output = mono_string_to_utf8(message);

		ENGINE_TRACE(output);
	}

	void ScriptGlue::Log_Info(MonoString* message)
	{
		std::string output = mono_string_to_utf8(message);

		ENGINE_INFO(output);
	}

	void ScriptGlue::Log_Warn(MonoString* message)
	{
		std::string output = mono_string_to_utf8(message);

		ENGINE_WARN(output);
	}

	void ScriptGlue::Log_Error(MonoString* message)
	{
		std::string output = mono_string_to_utf8(message);

		ENGINE_ERROR(output);
	}

	void ScriptGlue::Log_Critical(MonoString* message)
	{
		std::string output = mono_string_to_utf8(message);

		ENGINE_CRITICAL(output);
	}

#pragma endregion

}

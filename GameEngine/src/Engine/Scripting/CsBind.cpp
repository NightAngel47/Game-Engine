#include "enginepch.h"
#include "Engine/Scripting/CsBind.h"

void Engine::CsBind::CS_Log_Trace(MonoString* message)
{
	std::string output = mono_string_to_utf8(message);

	ENGINE_TRACE(output);
}

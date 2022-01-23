#pragma once

#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>
#include <mono/metadata/appdomain.h>
#include <mono/metadata/debug-helpers.h>

namespace Engine
{
	class CsBind
	{
	public:
		static void CS_Log_Trace(MonoString* message);
	};
}


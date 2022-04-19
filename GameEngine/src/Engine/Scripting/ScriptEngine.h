#pragma once

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>

namespace Engine
{
	class ScriptEngine
	{
	public:
		ScriptEngine();
		~ScriptEngine();

		

	private:
		static ScriptEngine* s_Instance;

		// Mono App
		MonoDomain* m_MonoDomain;
		MonoAssembly* m_MonoAssembly;
		MonoImage* m_MonoAssemblyImage;
	};
}

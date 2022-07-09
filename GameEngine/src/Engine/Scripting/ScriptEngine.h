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

		static ScriptEngine* s_Instance;

		MonoDomain* GetMonoDomain() { return m_MonoDomain; }
		MonoAssembly* GetMonoAssembly() { return m_MonoAssembly; }
		MonoImage* GetMonoImage() { return m_MonoImage; }

		static void HandleMonoException(MonoObject* ptrExObject);

	private:
		MonoDomain* m_MonoDomain;
		MonoAssembly* m_MonoAssembly;
		MonoImage* m_MonoImage;
	};
}

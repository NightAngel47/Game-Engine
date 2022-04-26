#pragma once
#include "Engine/Core/Timestep.h"

#include <mono/metadata/object-forward.h>

namespace Engine
{
	class MonoScript
	{
	public:
		MonoScript() = default;
		MonoScript(const std::string& namespaceName, const std::string& className);
		~MonoScript();

		void OnCreate();
		void OnDestroy();
		void OnUpdate(Timestep ts);
	private:
		MonoMethod* m_OnCreateMethodPtr;
		MonoMethod* m_OnDestroyMethodPtr;
		MonoMethod* m_OnUpdateMethodPtr;

		MonoObject* m_PtrGameObject;
		uint32_t m_GameObjectGCHandle = 0;
	};
}

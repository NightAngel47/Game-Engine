#pragma once

#include <entt.hpp>

#include "Engine/Core/Timestep.h"

namespace Engine
{
	class Scene
	{
	public:
		Scene();
		~Scene();

		// TEMP
		entt::registry& Reg() { return m_Registry; }

		entt::entity CreateEntity();
		
		void OnUpdate(Timestep ts);
	private:
		entt::registry m_Registry;
	};
	
}

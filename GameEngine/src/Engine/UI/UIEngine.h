#pragma once
#include "Engine/Core/UUID.h"
#include "Engine/Scene/Scene.h"

#include <glm/glm.hpp>

namespace Engine
{
	struct ScriptFieldInstance;
	struct ScriptMethod;

	struct Interaction
	{
		Interaction();

		UUID InteractedEntityID = UUID::INVALID();
		std::string InteractedFunction;
		ScriptFieldInstance* Params[8]{};

		void SetupParams(ScriptMethod scriptMethod);

		void Interacted();
	private:
		void* m_FunctionParams[8]{};
	};

	struct ButtonStates
	{
		bool Interactable = true;
		bool Hovered = false;
		bool Pressed = false;
	};

	class UIEngine
	{
	public:
		UIEngine() = delete;

		static void OnUIStart(Scene* scene);
		static void OnUIUpdate(Timestep ts, float viewportWidth, float viewportHeight);
		static void OnUIStop();

		static void SetViewportMousePos(int mouseX, int mouseY);
		static const glm::vec2 GetViewportMousePos();

		static bool IsOverElement(Entity entity, float viewportWidth, float viewportHeight);
	};

}

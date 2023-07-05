#include "enginepch.h"
#include "Engine/UI/UIEngine.h"
#include "Engine/Scripting/ScriptEngine.h"

#include <cstring>

namespace Engine
{
	struct UIEngineData
	{
		Scene* SceneContext = nullptr;

		glm::vec2 ViewportMousePos{ 0.0f };
	};

	static UIEngineData* s_UIEngineData = nullptr;

	void UIEngine::OnUIStart(Scene* scene)
	{
		ENGINE_PROFILE_FUNCTION();

		s_UIEngineData = new UIEngineData();
		s_UIEngineData->SceneContext = scene;
	}

	void UIEngine::OnUIUpdate(Timestep ts, float viewportWidth, float viewportHeight)
	{
		ENGINE_PROFILE_FUNCTION();

		auto view = s_UIEngineData->SceneContext->GetAllEntitiesWith<UILayoutComponent>();
		for (auto e : view)
		{
			if (!s_UIEngineData->SceneContext->IsEntityHandleValid(e))
				continue;
			
			Entity entity = { e, s_UIEngineData->SceneContext };
			bool isOver = UIEngine::IsOverElement(entity, viewportWidth, viewportHeight);

			if (entity.HasComponent<UIButtonComponent>())
			{
				auto& button = entity.GetComponent<UIButtonComponent>();
				button.ButtonState.Hovered = isOver;

				if (isOver && Input::IsMouseButtonPressed(Mouse::MouseCode::ButtonLeft))
					button.OnPressed();
				else if (!Input::IsMouseButtonPressed(Mouse::MouseCode::ButtonLeft))
					button.OnReleased();
			}
		}
	}

	void UIEngine::OnUIStop()
	{
		ENGINE_PROFILE_FUNCTION();

		delete s_UIEngineData;
	}

	void UIEngine::SetViewportMousePos(int mouseX, int mouseY)
	{
		s_UIEngineData->ViewportMousePos = { mouseX, mouseY };
	}

	const glm::vec2 UIEngine::GetViewportMousePos()
	{
		return s_UIEngineData->ViewportMousePos;
	}

	bool UIEngine::IsOverElement(Entity entity, float viewportWidth, float viewportHeight)
	{
		auto& layout = entity.GetComponent<UILayoutComponent>();

		glm::mat4 transform = entity.GetUISpaceTransform();
		glm::vec3 uiPos = Math::PositionFromTransform(transform);

		glm::vec2 halfViewportSize{ viewportWidth / 2, viewportHeight / 2 };

		float xHalfSize = layout.Size.x / 2;
		float yHalfSize = layout.Size.y / 2;

		glm::vec2 xRange{ uiPos.x - xHalfSize, uiPos.x + xHalfSize };
		xRange += halfViewportSize.x;
		glm::vec2 yRange{ uiPos.y - yHalfSize, uiPos.y + yHalfSize };
		yRange += halfViewportSize.y;

		glm::vec2 viewportMousePos = s_UIEngineData->ViewportMousePos;

		bool isOver = false;
		if (viewportMousePos.x >= xRange.x &&
			viewportMousePos.x <= xRange.y &&
			viewportMousePos.y >= yRange.x &&
			viewportMousePos.y <= yRange.y)
		{
			isOver = true;
		}

		return isOver;
	}

	Interaction::Interaction()
	{
		for (auto & Param : Params)
		{
			Param = new ScriptFieldInstance();
			Param->Field = { ScriptFieldType::None };
		}
	}


	void Interaction::ClearInteraction()
	{
		InteractedEntityID = UUID::INVALID();
		InteractedFunction.clear();
		ClearParams();
	}

	void Interaction::ClearParams()
	{
		for (int i = 0; i < 8; ++i)
		{
			Params[i] = nullptr;
			m_FunctionParams[i] = nullptr;
		}
	}

	void Interaction::SetupParams(ScriptMethod scriptMethod)
	{
		ClearParams();

		for (int i = 0; i < 8; ++i)
		{
			auto paramType = scriptMethod.ParamTypes[i];

			Params[i] = new ScriptFieldInstance();
			Params[i]->Field = { paramType };
			m_FunctionParams[i] = nullptr;
		}
	}

	void Interaction::Interacted()
	{
		if (!InteractedEntityID.IsValid())
			return;

		Entity interactedEntity = s_UIEngineData->SceneContext->GetEntityWithUUID(InteractedEntityID);
		ScriptComponent sc = interactedEntity.GetComponent<ScriptComponent>();
		auto scriptMethod = ScriptEngine::GetScriptMethodMap(sc.ClassName).at(InteractedFunction);

		auto& scriptInstance = ScriptEngine::GetEntityInstance(interactedEntity);
		auto& scriptClass = scriptInstance->GetScriptClass();

		int i = 0;
		auto paramType = Params[i]->Field.Type;
		while (i < 8 && paramType != ScriptFieldType::None)
		{
			if (paramType == ScriptFieldType::Char)
			{
				char val[2];
				memset(val, 0, sizeof(val));
				val[0] = Params[i]->GetValue<char>();

				auto monoString = ScriptEngine::StringToMonoString(val);
				m_FunctionParams[i] = monoString;
			}
			else if (paramType == ScriptFieldType::String)
			{
				std::string val = Params[i]->GetValue<std::string>();
				auto monoString = ScriptEngine::StringToMonoString(val);
				m_FunctionParams[i] = monoString;
			}
			else
			{
				void* val = Params[i]->GetValue<void*>();
				m_FunctionParams[i] = &val;
			}

			++i;
			paramType = Params[i]->Field.Type;
		}

		scriptClass->InvokeMethod(scriptInstance->GetMonoObject(), scriptMethod.ClassMethod, m_FunctionParams);
	}

}

#include "enginepch.h"
#include "Engine/Scripting/ScriptGlue.h"
#include <Engine/Scripting/ScriptEngine.h>

#include "Engine/Scene/Components.h"

namespace InternalCalls
{
	struct RuntimeData
	{
		Engine::Scene* m_ActiveScene;
	};

	static RuntimeData* s_Data;

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

#pragma region Entity

		mono_add_internal_call("Engine.Core.InternalCalls::Entity_GetComponent(ulong,Engine.Scene.TagComponent/TagData&)", &Entity_GetComponent_Tag);
		mono_add_internal_call("Engine.Core.InternalCalls::Entity_GetComponent(ulong,Engine.Scene.TransformComponent/TransformData&)", &Entity_GetComponent_Transform);

#pragma endregion

#pragma region Transform Component

		mono_add_internal_call("Engine.Core.InternalCalls::TransformComponent_SetPosition(ulong,single&,single&,single&)", &TransformComponent_SetPosition);

#pragma endregion

	}

	void ScriptGlue::InitRuntime(Engine::Scene* activeScene)
	{
		s_Data = new RuntimeData();

		s_Data->m_ActiveScene = activeScene;
	}

	void ScriptGlue::ShutdownRuntime()
	{
		s_Data->m_ActiveScene = nullptr;

		delete s_Data;
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

#pragma region Entity

	void ScriptGlue::Entity_GetComponent_Tag(uint64_t entityID, TagData* data)
	{
		auto view = s_Data->m_ActiveScene->GetAllEntitiesWith<Engine::IDComponent, Engine::TagComponent>();

		for (auto entity : view)
		{
			auto [uuid, tag] = view.get<Engine::IDComponent, Engine::TagComponent>(entity);
			if (uuid.ID == entityID)
			{
				TagData componentData{};

				componentData.tag = mono_string_new(Engine::ScriptEngine::GetAppDomain(), tag.Tag.c_str());

				*data = componentData;

				return;
			}
		}
	}

	void ScriptGlue::Entity_GetComponent_Transform(uint64_t entityID, TransformData* data)
	{
		auto view = s_Data->m_ActiveScene->GetAllEntitiesWith<Engine::IDComponent, Engine::TransformComponent>();

		for (auto entity : view)
		{
			auto [uuid, tc] = view.get<Engine::IDComponent, Engine::TransformComponent>(entity);
			if (uuid.ID == entityID)
			{
				TransformData td{};

				//td.position[0] = tc.Position.x;
				//td.position[1] = tc.Position.y;
				//td.position[2] = tc.Position.z;
				//
				//td.rotation[0] = tc.Rotation.x;
				//td.rotation[1] = tc.Rotation.y;
				//td.rotation[2] = tc.Rotation.z;
				//
				//td.scale[0] = tc.Scale.x;
				//td.scale[1] = tc.Scale.y;
				//td.scale[2] = tc.Scale.z;

				td.posX = tc.Position.x;
				td.posY = tc.Position.y;
				td.posZ = tc.Position.z;

				td.rotX = tc.Rotation.x;
				td.rotY = tc.Rotation.y;
				td.rotZ = tc.Rotation.z;

				td.scaleX = tc.Scale.x;
				td.scaleY = tc.Scale.y;
				td.scaleZ = tc.Scale.z;

				*data = td;
				return;
			}
		}
	}

#pragma endregion

#pragma region Transform Component

	void ScriptGlue::TransformComponent_SetPosition(uint64_t entityID, float& x, float& y, float& z)
	{
		auto view = s_Data->m_ActiveScene->GetAllEntitiesWith<Engine::IDComponent, Engine::TransformComponent>();

		for (auto entity : view)
		{
			auto [uuid, tc] = view.get<Engine::IDComponent, Engine::TransformComponent>(entity);
			if (uuid.ID == entityID)
			{
				tc.Position = { x, y, z };

				return;
			}
		}
	}

#pragma endregion
}

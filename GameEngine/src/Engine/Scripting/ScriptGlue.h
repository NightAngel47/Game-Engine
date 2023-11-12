#pragma once
#include "Engine/Core/UUID.h"
#include "Engine/Core/KeyCodes.h"
#include "Engine/Core/MouseCodes.h"
#include "Engine/Scene/Components.h"

#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>
#include <mono/metadata/appdomain.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/reflection.h>

#include <glm/glm.hpp>

namespace InternalCalls
{
	class ScriptGlue
	{
	public:
		static void RegisterInternalCalls();
		static void RegisterComponentTypes();

	private:
#pragma region Application

		static void Application_Quit();

#pragma endregion Application

#pragma region Log

		static void Log_Trace(MonoString* message);
		static void Log_Info(MonoString* message);
		static void Log_Warn(MonoString* message);
		static void Log_Error(MonoString* message);
		static void Log_Critical(MonoString* message);

#pragma endregion Log

#pragma region Input

		static bool Input_IsKeyPressed(Engine::KeyCode key);
		static bool Input_IsMouseButtonPressed(Engine::MouseCode key);

		static void Input_GetMousePosition(glm::vec2* mousePos);

		static float Input_GetMouseY();
		static float Input_GetMouseX();

#pragma endregion Input

#pragma region Random

		static float Random_Float();
		static float Random_Float_Seed(unsigned int seed);

		static int Random_Int();
		static int Random_Int_Seed(unsigned int seed);

		static float Random_Range_Float(float min, float max);
		static float Random_Range_Float_Seed(float min, float max, unsigned int seed);
		static int Random_Range_Int(int min, int max);
		static int Random_Range_Int_Seed(int min, int max, unsigned int seed);

#pragma endregion Random

#pragma region Vector2

		static float Vector2_Magnitude(glm::vec2& vector2);
		static float Vector2_SqrMagnitude(glm::vec2& vector2);
		static void Vector2_Normalize(glm::vec2* vector2);
		static void Vector2_RotateAroundAxis(glm::vec2* vector2, float angle);

#pragma endregion Vector2

#pragma region Vector3

		static float Vector3_Magnitude(glm::vec3& vector3);
		static float Vector3_SqrMagnitude(glm::vec3& vector3);
		static void Vector3_Normalize(glm::vec3* vector3);
		static void Vector3_RotateAroundAxis(glm::vec3* vector3, float angle, glm::vec3& axis);

#pragma endregion Vector3

#pragma region Vector4

		static float Vector4_Magnitude(glm::vec4& vector4);
		static float Vector4_SqrMagnitude(glm::vec4& vector4);
		static void Vector4_Normalize(glm::vec4* vector4);
		static void Vector4_RotateAroundAxis(glm::vec4* vector4, float angle, glm::vec3& axis);

#pragma endregion Vector4

#pragma region Physics2DContact

		static uint64_t Physics2DContact_GetEntityByID(Engine::UUID entityID);

#pragma endregion Physics2DContact

#pragma region SceneManager

		static void SceneManager_LoadSceneByHandle(Engine::AssetHandle handle);
		static void SceneManager_LoadSceneByPath(MonoString* path);

#pragma endregion SceneManager

#pragma region Entity

		static MonoString* Entity_GetName(Engine::UUID entityID);

		static bool Entity_HasComponent(Engine::UUID entityID, MonoReflectionType* componentType);
		static void Entity_AddComponent(Engine::UUID entityID, MonoReflectionType* componentType);

		static uint64_t Entity_FindEntityByName(MonoString* name);
		static uint64_t Entity_CreateEntity(MonoString* name);
		static uint64_t Entity_InstantiatePrefab(Engine::AssetHandle prefabID);
		static MonoObject* Entity_GetScriptInstance(Engine::UUID entityID);
		static void Entity_DestroyEntity(Engine::UUID entityID);

		static uint64_t Entity_GetParent(Engine::UUID entityID);
		static void Entity_SetParent(Engine::UUID entityID, Engine::UUID parentID);
		static MonoArray* Entity_GetChildren(Engine::UUID entityID);

		static void Entity_GetWorldTransformPosition(Engine::UUID entityID, glm::vec3* position);
		static void Entity_GetUITransformPosition(Engine::UUID entityID, glm::vec3* position);

#pragma endregion Entity

#pragma region TransformComponent

		static void TransformComponent_GetPosition(Engine::UUID entityID, glm::vec3* position);
		static void TransformComponent_SetPosition(Engine::UUID entityID, glm::vec3& position);

		static void TransformComponent_GetRotation(Engine::UUID entityID, glm::vec3* rotation);
		static void TransformComponent_SetRotation(Engine::UUID entityID, glm::vec3& rotation);

		static void TransformComponent_GetScale(Engine::UUID entityID, glm::vec3* scale);
		static void TransformComponent_SetScale(Engine::UUID entityID, glm::vec3& scale);

		static void TransformComponent_GetUp(Engine::UUID entityID, glm::vec3* up);
		static void TransformComponent_GetRight(Engine::UUID entityID, glm::vec3* right);
		static void TransformComponent_GetForward(Engine::UUID entityID, glm::vec3* forward);

#pragma endregion TransformComponent

#pragma region SpriteRendererComponent

		static void SpriteRendererComponent_GetColor(Engine::UUID entityID, glm::vec4* color);
		static void SpriteRendererComponent_SetColor(Engine::UUID entityID, glm::vec4& color);

		static float SpriteRendererComponent_GetTiling(Engine::UUID entityID);
		static void SpriteRendererComponent_SetTiling(Engine::UUID entityID, float tiling);

#pragma endregion SpriteRendererComponent

#pragma region CircleRendererComponent

		static void CircleRendererComponent_GetColor(Engine::UUID entityID, glm::vec4* color);
		static void CircleRendererComponent_SetColor(Engine::UUID entityID, glm::vec4& color);

		static float CircleRendererComponent_GetThickness(Engine::UUID entityID);
		static void CircleRendererComponent_SetThickness(Engine::UUID entityID, float thickness);

		static float CircleRendererComponent_GetFade(Engine::UUID entityID);
		static void CircleRendererComponent_SetFade(Engine::UUID entityID, float fade);

#pragma endregion CircleRendererComponent

#pragma region TextRendererComponent

		static void TextRendererComponent_GetColor(Engine::UUID entityID, glm::vec4* color);
		static void TextRendererComponent_SetColor(Engine::UUID entityID, glm::vec4& color);

		static MonoString* TextRendererComponent_GetText(Engine::UUID entityID);
		static void TextRendererComponent_SetText(Engine::UUID entityID, MonoString* text);

		static float TextRendererComponent_GetKerning(Engine::UUID entityID);
		static void TextRendererComponent_SetKerning(Engine::UUID entityID, float kerning);

		static float TextRendererComponent_GetLineSpacing(Engine::UUID entityID);
		static void TextRendererComponent_SetLineSpacing(Engine::UUID entityID, float lineSpacing);

#pragma endregion TextRendererComponent

#pragma region Rigidbody2DComponent

		static void Rigidbody2DComponent_GetType(Engine::UUID entityID, Engine::Rigidbody2DComponent::BodyType* bodyType);
		static void Rigidbody2DComponent_SetType(Engine::UUID entityID, Engine::Rigidbody2DComponent::BodyType bodyType);

		static void Rigidbody2DComponent_GetLinearVelocity(Engine::UUID entityID, glm::vec2* velocity);
		static void Rigidbody2DComponent_SetLinearVelocity(Engine::UUID entityID, glm::vec2& velocity);

		static float Rigidbody2DComponent_GetGravityScale(Engine::UUID entityID);
		static void Rigidbody2DComponent_SetGravityScale(Engine::UUID entityID, float gravityScale);

		static void Rigidbody2DComponent_ApplyLinearImpulse(Engine::UUID entityID, glm::vec2& impulse, glm::vec2& worldPosition, bool wake);
		static void Rigidbody2DComponent_ApplyLinearImpulseToCenter(Engine::UUID entityID, glm::vec2& impulse, bool wake);

		static void Rigidbody2DComponent_ApplyForce(Engine::UUID entityID, glm::vec2& force, glm::vec2& worldPosition, bool wake);
		static void Rigidbody2DComponent_ApplyForceToCenter(Engine::UUID entityID, glm::vec2& force, bool wake);

#pragma endregion Rigidbody2DComponent

#pragma region CameraComponent

		static float CameraComponent_GetOrthographicSize(Engine::UUID entityID);
		static void CameraComponent_SetOrthographicSize(Engine::UUID entityID, float size);

#pragma endregion CameraComponent

#pragma region ScriptComponent

		static MonoString* ScriptComponent_GetClassName(Engine::UUID entityID);
		static void ScriptComponent_InstantiateClass(Engine::UUID entityID, MonoString* className);

#pragma endregion ScriptComponent
	};

}


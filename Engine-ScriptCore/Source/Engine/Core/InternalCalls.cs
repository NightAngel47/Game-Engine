using System.Runtime.CompilerServices;
using System;
using Engine.Math;

namespace Engine.Core
{
	internal class InternalCalls
	{
		#region Application

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Application_Quit();

		#endregion Application

		#region Log

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Log_Trace(string message);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Log_Info(string message);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Log_Warn(string message);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Log_Error(string message);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Log_Critical(string message);

		#endregion Log

		#region Input

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool Input_IsKeyPressed(int key);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool Input_IsMouseButtonPressed(int key);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Input_GetMousePosition(out Vector2 mousePos);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float Input_GetMouseY();

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float Input_GetMouseX();

		#endregion Input

		#region Random

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float Random_Float();

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float Random_Float_Seed(uint seed);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern int Random_Int();

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern int Random_Int_Seed(uint seed);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float Random_Range_Float(float min, float max);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float Random_Range_Float_Seed(float min, float max, uint seed);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern int Random_Range_Int(int min, int max);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern int Random_Range_Int_Seed(int min, int max, uint seed);

		#endregion Random

		#region Vector2

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float Vector2_Magnitude(ref Vector2 vector2);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float Vector2_SqrMagnitude(ref Vector2 vector2);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Vector2_Normalize(out Vector2 vector2);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Vector2_RotateAroundAxis(out Vector2 vector2, float angle);

		#endregion Vector2

		#region Vector3

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float Vector3_Magnitude(ref Vector3 vector3);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float Vector3_SqrMagnitude(ref Vector3 vector3);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Vector3_Normalize(out Vector3 vector3);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Vector3_RotateAroundAxis(out Vector3 vector3, float angle, ref Vector3 axis);

		#endregion Vector3

		#region Vector4

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float Vector4_Magnitude(ref Vector4 vector4);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float Vector4_SqrMagnitude(ref Vector4 vector4);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Vector4_Normalize(out Vector4 vector4);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Vector4_RotateAroundAxis(out Vector4 vector4, float angle, ref Vector3 axis);

		#endregion Vector4

		#region Physics2DContact

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern ulong Physics2DContact_GetEntityByID(ulong entityID);

		#endregion Physics2DContact

		#region SceneManager

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void SceneManager_LoadSceneByHandle(ulong handle);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void SceneManager_LoadSceneByPath(string path);

		#endregion SceneManager

		#region Entity

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern string Entity_GetName(ulong entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool Entity_HasComponent(ulong entityID, System.Type componentType);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Entity_AddComponent(ulong entityID, System.Type componentType);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern ulong Entity_FindEntityByName(string name);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern ulong Entity_CreateEntity(string name);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern object Entity_GetScriptInstance(ulong entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Entity_DestroyEntity(ulong entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern ulong Entity_GetParent(ulong entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Entity_SetParent(ulong entityID, ulong parentID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern ulong[] Entity_GetChildren(ulong entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Entity_GetWorldTransformPosition(ulong entityID, out Vector3 position);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Entity_GetUITransformPosition(ulong entityID, out Vector3 position);

		#endregion Entity

		#region TransformComponent

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void TransformComponent_GetPosition(ulong entityID, out Vector3 position);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void TransformComponent_SetPosition(ulong entityID, ref Vector3 position);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void TransformComponent_GetRotation(ulong entityID, out Vector3 rotation);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void TransformComponent_SetRotation(ulong entityID, ref Vector3 rotation);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void TransformComponent_GetScale(ulong entityID, out Vector3 scale);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void TransformComponent_SetScale(ulong entityID, ref Vector3 scale);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void TransformComponent_GetUp(ulong entityID, out Vector3 up);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void TransformComponent_GetRight(ulong entityID, out Vector3 right);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void TransformComponent_GetForward(ulong entityID, out Vector3 forward);

		#endregion TransformComponent

		#region SpriteRendererComponent

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void SpriteRendererComponent_GetColor(ulong entityID, out Vector4 color);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void SpriteRendererComponent_SetColor(ulong entityID, ref Vector4 color);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float SpriteRendererComponent_GetTiling(ulong entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void SpriteRendererComponent_SetTiling(ulong entityID, float tiling);

		#endregion SpriteRendererComponent

		#region CircleRendererComponent

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void CircleRendererComponent_GetColor(ulong entityID, out Vector4 color);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void CircleRendererComponent_SetColor(ulong entityID, ref Vector4 color);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float CircleRendererComponent_GetThinkness(ulong entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void CircleRendererComponent_SetThinkness(ulong entityID, float thickness);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float CircleRendererComponent_GetFade(ulong entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void CircleRendererComponent_SetFade(ulong entityID, float fade);

		#endregion CircleRendererComponent

		#region TextRendererComponent

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void TextRendererComponent_GetColor(ulong entityID, out Vector4 color);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void TextRendererComponent_SetColor(ulong entityID, ref Vector4 color);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern string TextRendererComponent_GetText(ulong entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void TextRendererComponent_SetText(ulong entityID, string thickness);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float TextRendererComponent_GetKerning(ulong entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void TextRendererComponent_SetKerning(ulong entityID, float kerning);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float TextRendererComponent_GetLineSpacing(ulong entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void TextRendererComponent_SetLineSpacing(ulong entityID, float lineSpacing);

		#endregion TextRendererComponent

		#region Rigidbody2DComponent

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Rigidbody2DComponent_GetType(ulong entityID, out int bodyType);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Rigidbody2DComponent_SetType(ulong entityID, int bodyType);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Rigidbody2DComponent_GetLinearVelocity(ulong entityID, out Vector2 velocity);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Rigidbody2DComponent_SetLinearVelocity(ulong entityID, ref Vector2 velocity);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float Rigidbody2DComponent_GetGravityScale(ulong entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Rigidbody2DComponent_SetGravityScale(ulong entityID, float gravityScale);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Rigidbody2DComponent_ApplyLinearImpulse(ulong entityID, ref Vector2 impulse, ref Vector2 worldPosition, bool wake);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Rigidbody2DComponent_ApplyLinearImpulseToCenter(ulong entityID, ref Vector2 impulse, bool wake);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Rigidbody2DComponent_ApplyForce(ulong entityID, ref Vector2 force, ref Vector2 worldPosition, bool wake);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Rigidbody2DComponent_ApplyForceToCenter(ulong entityID, ref Vector2 force, bool wake);

		#endregion Rigidbody2DComponent

		#region CameraComponent

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float CameraComponent_GetOrthographicSize(ulong entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void CameraComponent_SetOrthographicSize(ulong entityID, float size);

		#endregion CameraComponent

		#region ScriptComponent

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void ScriptComponent_InstantiateClass(ulong entityID, string className);

		#endregion ScriptComponent
	}
}

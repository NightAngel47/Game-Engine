using System.Runtime.CompilerServices;
using Engine.Math;

namespace Engine.Core
{
	internal class InternalCalls
	{
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
		internal static extern float Vector2_sqrMagnitude(ref Vector2 vector2);

		#endregion Vector2

		#region Vector3

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float Vector3_Magnitude(ref Vector3 vector3);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float Vector3_sqrMagnitude(ref Vector3 vector3);

		#endregion Vector3

		#region Vector4

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float Vector4_Magnitude(ref Vector4 vector4);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float Vector4_sqrMagnitude(ref Vector4 vector4);

		#endregion Vector4

		#region Entity

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

		#region Rigidbody2DComponent

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Rigidbody2DComponent_GetLinearVelocity(ulong entityID, out Vector2 velocity);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Rigidbody2DComponent_ApplyLinearImpulse(ulong entityID, ref Vector2 impulse, ref Vector2 worldPosition, bool wake);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Rigidbody2DComponent_ApplyLinearImpulseToCenter(ulong entityID, ref Vector2 impulse, bool wake);
		
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Rigidbody2DComponent_GetType(ulong entityID, out int bodyType);
		
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Rigidbody2DComponent_SetType(ulong entityID, int bodyType);

		#endregion Rigidbody2DComponent
	}
}

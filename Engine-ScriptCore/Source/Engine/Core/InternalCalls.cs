using System;
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

		#endregion

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

		#endregion

		#region Entity

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool Entity_HasComponent(ulong entityID, Type componentType);

		#endregion

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

		#endregion

		#region SpriteRendererComponent

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void SpriteRendererComponent_GetColor(ulong entityID, out Vector4 color);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void SpriteRendererComponent_SetColor(ulong entityID, ref Vector4 color);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float SpriteRendererComponent_GetTiling(ulong entityID);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void SpriteRendererComponent_SetTiling(ulong entityID, float tiling);

		#endregion
	}
}

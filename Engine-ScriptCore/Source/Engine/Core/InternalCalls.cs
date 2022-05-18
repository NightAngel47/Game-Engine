using Engine.Scene;
using System.Runtime.CompilerServices;

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
		internal static extern void Input_GetMousePosition(out float x, out float y);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float Input_GetMouseY();

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float Input_GetMouseX();

		#endregion

		#region Entity

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Entity_GetComponent(ulong entityID, out TagComponent.TagData data);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Entity_GetComponent(ulong entityID, out TransformComponent.TransformData data);
		
		#endregion
	}
}

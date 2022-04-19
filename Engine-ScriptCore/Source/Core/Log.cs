using System.Runtime.CompilerServices;

namespace Engine.Source.Core
{
	#region Log

	internal class Log
	{

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

	}

	#endregion
}

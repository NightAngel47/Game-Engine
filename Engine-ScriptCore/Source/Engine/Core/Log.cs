namespace Engine.Core
{
	public class Log
	{
		public static void Trace(string message) => InternalCalls.Log_Trace(message);
		public static void Info(string message) => InternalCalls.Log_Info(message);
		public static void Warn(string message) => InternalCalls.Log_Warn(message);
		public static void Error(string message) => InternalCalls.Log_Error(message);
		public static void Critical(string message) => InternalCalls.Log_Critical(message);
	}
}

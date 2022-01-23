using System.Runtime.CompilerServices;

namespace GEL
{
	public class Log
	{
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public extern static void Trace(string message);
	}
}

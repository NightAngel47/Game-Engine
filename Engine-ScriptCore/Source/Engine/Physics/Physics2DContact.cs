using System.Runtime.InteropServices;

using Engine.Scene;
using Engine.Core;

namespace Engine.Physics
{
	[StructLayout(LayoutKind.Sequential)]
	public readonly struct Physics2DContact
	{
		internal readonly ulong otherEntityID;
		internal readonly ulong thisEntityID;

		public Entity OtherEntity => new Entity(InternalCalls.Physics2DContact_GetEntityByID(otherEntityID));
		public Entity ThisEntity => new Entity(InternalCalls.Physics2DContact_GetEntityByID(thisEntityID));
	}
}

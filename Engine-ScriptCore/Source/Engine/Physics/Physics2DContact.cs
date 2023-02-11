using System.Runtime.InteropServices;

using Engine.Scene;
using Engine.Core;

namespace Engine.Physics
{
	[StructLayout(LayoutKind.Sequential)]
	public readonly struct Physics2DContact
	{
		internal readonly ulong colliderEntityID;
		internal readonly ulong otherEntityID;

		public Entity ColliderEntity => new Entity(InternalCalls.Physics2DContact_GetEntityByID(colliderEntityID));
		public Entity OtherEntity => new Entity(InternalCalls.Physics2DContact_GetEntityByID(otherEntityID));
	}
}

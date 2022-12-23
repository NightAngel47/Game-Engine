using Engine.Core;
using Engine.Math;
using Engine.Scene;

namespace GameProject.Source
{
	public class VariableValidator : Entity
	{
		public bool Var0		= false;
		public char Var1		= 'Y';
		public string Var2		= "Hello There!";
		public float Var3		= 192.123f;
		public double Var4		= 1237.1232f;
		public sbyte Var5		= -96;
		public byte Var6		= 240;
		public short Var7		= -2400;
		public ushort Var8		= 2400;
		public int Var9			= -21654;
		public uint Var10		= 31654;
		public long Var11		= -3165;
		public ulong Var12		= 6316545;
		public Vector2 Var13	= new(1234, -1234);
		public Vector3 Var14	= new(-1234, 1234, 98723);
		public Vector4 Var15	= new(1234, 1234, -98723, 12356);
		public Entity Var16		= null;

		protected override void OnCreate()
		{
			if (Var0)
			{
				PrintVar(Var0);
				PrintVar(Var1);
				PrintVar(Var2);
				PrintVar(Var3);
				PrintVar(Var4);
				PrintVar(Var5);
				PrintVar(Var6);
				PrintVar(Var7);
				PrintVar(Var8);
				PrintVar(Var9);
				PrintVar(Var10);
				PrintVar(Var11);
				PrintVar(Var12);
				PrintVar(Var13);
				PrintVar(Var14);
				PrintVar(Var15);
				PrintVar(Var16);
			}
		}

		void PrintVar<T>(T var)
		{
			if (var == null)
				return;

			if (var.GetType().Equals("System.String") || var.GetType().Equals("System.Char"))
			{
				Log.Trace($"Var: {var.GetType()} = " + var);
			}
			else
			{
				Log.Trace($"Var: {var.GetType()} = {var}");
			}
		}
	}
}

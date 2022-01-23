using System;
using GEL;

namespace GES
{
	public class TestGame : IGame
	{
		public void tick()
		{
			Console.WriteLine("Hello Console World");
			Log.Trace("Logging Tick");
		}
	}
}

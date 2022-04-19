using GEL;

namespace GES
{
    public class GameMain
    {
		public static IGame main()
		{
			Log.Trace("Game Scripting Started");
			return new TestGame();
		}
    }
}

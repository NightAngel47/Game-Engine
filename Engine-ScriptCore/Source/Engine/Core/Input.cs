using Engine.Math;

namespace Engine.Core
{
	public class Input
	{
		public static bool IsKeyPressed(KeyCode key) => InternalCalls.Input_IsKeyPressed((int)key);
		public static bool IsMouseButtonPressed(MouseCode key) => InternalCalls.Input_IsMouseButtonPressed((int)key);
		
		public static Vector2 GetMousePosition()
		{
			InternalCalls.Input_GetMousePosition(out float x, out float y);
			return new Vector2(x, y);
		}

		public static float GetMouseY() => InternalCalls.Input_GetMouseY();
		public static float GetMouseX() => InternalCalls.Input_GetMouseX();
	}
}

using Engine.Scene;
using Engine.Core;
using Engine.Math;

namespace Source
{
	public class ExampleEntity : Entity
	{
		private float num;

		protected override void OnCreate()
		{
			Log.Info("Example Entity Created.");
		}

		protected override void OnDestroy()
		{
			Log.Warn("Example Entity Destroyed.");
		}

		protected override void OnUpdate(float ts)
		{
			if (Input.IsKeyPressed(KeyCode.Space))
			{
				num += ts;

				Log.Trace($"Example Entity num: {num}.");
			}

			if (Input.IsMouseButtonPressed(MouseCode.ButtonLeft))
			{
				Vector2 curMousePos = Input.GetMousePosition();

				Log.Trace($"Current Mouse Pos: ({curMousePos.x}, {curMousePos.y})");
			}
		}
	}
}

using Engine.Scene;
using Engine.Core;
using Engine.Math;

namespace GameProject.Source
{
	public class Player : Entity
	{
		public float moveSpeed = 5.0f;
		public float rotSpeeed = 5.0f;

		public Vector2 movement;
		public Vector3 rotation;

		public string exampleString = "Example";

		protected override void OnCreate()
		{

			Vector3 pos = Transform.Position;
			rotation = Transform.Rotation;
			Vector3 scale = Transform.Scale;
			Log.Info($"Position: ({pos.X}, {pos.Y}, {pos.Z})");
			Log.Info($"Rotation: ({rotation.X}, {rotation.Y}, {rotation.Z})");
			Log.Info($"Scale: ({scale.X}, {scale.Y}, {scale.Z})");
		}

		protected override void OnDestroy()
		{
			Log.Warn("Example Entity Destroyed.");
		}

		protected override void OnUpdate(float ts)
		{
			if (Input.IsMouseButtonPressed(MouseCode.ButtonMiddle))
			{
				Vector2 curMousePos = Input.GetMousePosition();

				Log.Trace($"Current Mouse Pos: ({curMousePos.X}, {curMousePos.Y})");
			}

			movement = new Vector2(0.0f, 0.0f);

			if (Input.IsKeyPressed(KeyCode.A))
			{
				movement.X += -1.0f * moveSpeed * ts;
			}

			if (Input.IsKeyPressed(KeyCode.D))
			{
				movement.X += 1.0f * moveSpeed * ts;
			}

			if (Input.IsKeyPressed(KeyCode.S))
			{
				movement.Y += -1.0f * moveSpeed * ts;
			}

			if (Input.IsKeyPressed(KeyCode.W))
			{
				movement.Y += 1.0f * moveSpeed * ts;
			}

			if (Input.IsKeyPressed(KeyCode.Q))
			{
				rotation = Transform.Rotation;
				rotation.Z += 1.0f * rotSpeeed * ts;
				Transform.Rotation = rotation;
			}

			if (Input.IsKeyPressed(KeyCode.E))
			{
				rotation = Transform.Rotation;
				rotation.Z += -1.0f * rotSpeeed * ts;
				Transform.Rotation = rotation;
			}

			if (movement.sqrMagnitude > 0)
			{
				Transform.Position += new Vector3(movement.X, movement.Y, 0.0f);
			}
		}
	}
}

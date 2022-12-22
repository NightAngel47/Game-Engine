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

		public float zoomSpeed = 1.0f;
		public Camera camera = null;

		public string exampleString = "Example";

		private Rigidbody2DComponent rb2d = null;

		protected override void OnCreate()
		{
			rb2d = GetComponent<Rigidbody2DComponent>();
		}

		protected override void OnUpdate(float ts)
		{
			camera ??= FindEntityByName("Camera").As<Camera>();

			if (Input.IsMouseButtonPressed(MouseCode.ButtonRight))
			{
				Vector2 curMousePos = Input.GetMousePosition();

				Log.Trace($"Current Mouse Pos: {curMousePos}");
			}

			movement = new Vector2();

			if (Input.IsKeyPressed(KeyCode.A))
			{
				movement.X += -1.0f;
			}

			if (Input.IsKeyPressed(KeyCode.D))
			{
				movement.X += 1.0f;
			}

			if (Input.IsKeyPressed(KeyCode.S))
			{
				movement.Y += -1.0f;
			}

			if (Input.IsKeyPressed(KeyCode.W))
			{
				movement.Y += 1.0f;
			}
			
			if (movement.sqrMagnitude > 0)
			{
				rb2d.ApplyLinearImpulse(movement * moveSpeed * ts, true);
			}

			if (Input.IsKeyPressed(KeyCode.Q))
			{
				Vector3 rotation = Transform.Rotation;
				rotation.Z += 1.0f * rotSpeeed * ts;
				Transform.Rotation = rotation;
			}

			if (Input.IsKeyPressed(KeyCode.E))
			{
				Vector3 rotation = Transform.Rotation;
				rotation.Z += -1.0f * rotSpeeed * ts;
				Transform.Rotation = rotation;
			}

			if (camera != null)
			{
				camera.DistanceFromPlayer += zoomSpeed * ts;
			}
			else
			{
				Log.Error("Camera is null!");
			}
		}
	}
}

using Engine.Scene;
using Engine.Core;
using Engine.Math;

namespace Source
{
	public class Camera : Entity
	{
		public float moveSpeed = 5.0f;
		public float rotSpeeed = 5.0f;

		protected override void OnUpdate(Timestep ts)
		{

			Vector2 movement = new Vector2(0.0f, 0.0f);

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

			Transform.Position += new Vector3(movement.X, movement.Y, 0.0f);
		}
	}
}

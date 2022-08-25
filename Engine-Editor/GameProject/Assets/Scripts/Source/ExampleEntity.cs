using Engine.Scene;
using Engine.Core;
using Engine.Math;

namespace Source
{
	public class ExampleEntity : Entity
	{
		private TransformComponent transformComponent;
		public float speed = 5.0f;
		public int testNum = 25;

		private float timeProgress;
		private int iterations;

		public string exampleString = "Example";

		protected override void OnCreate()
		{
			//TagComponent tagComponent = GetComponent<TagComponent>();
			//string name = tagComponent.Tag;
			//Log.Info($"Name: {name}");

			transformComponent = GetComponent<TransformComponent>();
			Vector3 pos = transformComponent.Position;
			Vector3 rot = transformComponent.Rotation;
			Vector3 scale = transformComponent.Scale;
			Log.Info($"Position: ({pos.X}, {pos.Y}, {pos.Z})");
			Log.Info($"Rotation: ({rot.X}, {rot.Y}, {rot.Z})");
			Log.Info($"Scale: ({scale.X}, {scale.Y}, {scale.Z})");

			Log.Warn($"Speed: {speed}");
		}

		protected override void OnDestroy()
		{
			Log.Warn("Example Entity Destroyed.");
		}

		protected override void OnUpdate(Timestep ts)
		{
			if (Input.IsKeyPressed(KeyCode.Space))
			{
				Log.Trace($"{ts}");
			}

			timeProgress += ts;
			++iterations;
			if (timeProgress >= 1.0f)
			{
				Log.Trace($"Script Calls per second: {iterations}");
				timeProgress = 0.0f;
				iterations = 0;
			}

			if (Input.IsMouseButtonPressed(MouseCode.ButtonMiddle))
			{
				Vector2 curMousePos = Input.GetMousePosition();

				Log.Trace($"Current Mouse Pos: ({curMousePos.X}, {curMousePos.Y})");
			}

			Vector2 movement = new Vector2(0.0f, 0.0f);

			if (Input.IsKeyPressed(KeyCode.A))
			{
				movement.X += -1.0f * speed * ts;
				//Log.Trace($"Movement: ({movement.X}, {movement.Y})");
			}

			if (Input.IsKeyPressed(KeyCode.D))
			{
				movement.X += 1.0f * speed * ts;
				//Log.Trace($"Movement: ({movement.X}, {movement.Y})");
			}

			if (Input.IsKeyPressed(KeyCode.S))
			{
				movement.Y += -1.0f * speed * ts;
				//Log.Trace($"Movement: ({movement.X}, {movement.Y})");
			}

			if (Input.IsKeyPressed(KeyCode.W))
			{
				movement.Y += 1.0f * speed * ts;
				//Log.Trace($"Movement: ({movement.X}, {movement.Y})");
			}

			transformComponent.Position += new Vector3(movement.X, movement.Y, 0.0f);
			//Vector3 pos = transformComponent.Position;
			//Log.Info($"Position: ({pos.X}, {pos.Y}, {pos.Z})");
		}
	}
}

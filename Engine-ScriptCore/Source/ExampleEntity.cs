using Engine.Scene;
using Engine.Core;
using Engine.Math;

namespace Source
{
	public class ExampleEntity : Entity
	{
		private float num;
		private TransformComponent transformComponent;
		//private float speed = 5.0f;

		protected override void OnCreate()
		{
			Log.Info($"Entity Created with ID: {ID}");

			TagComponent tagComponent = GetComponent<TagComponent>();
			string name = tagComponent.Tag;
			Log.Info($"Name: {name}");

			transformComponent = GetComponent<TransformComponent>();
			Vector3 pos = transformComponent.Position;
			Vector3 rot = transformComponent.Rotation;
			Vector3 scale = transformComponent.Scale;
			Log.Info($"Position: ({pos.X}, {pos.Y}, {pos.Z})");
			Log.Info($"Rotation: ({rot.X}, {rot.Y}, {rot.Z})");
			Log.Info($"Scale: ({scale.X}, {scale.Y}, {scale.Z})");
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

				Log.Trace($"Example Entity num: {num}");
			}

			if (Input.IsMouseButtonPressed(MouseCode.ButtonLeft))
			{
				transformComponent.Position += new Vector3(-5.0f * ts, 0, 0);

				Vector3 pos = transformComponent.Position;
				Log.Info($"Position: ({pos.X}, {pos.Y}, {pos.Z})");
			}

			if (Input.IsMouseButtonPressed(MouseCode.ButtonRight))
			{
				transformComponent.Position += new Vector3(5.0f * ts, 0, 0);

				Vector3 pos = transformComponent.Position;
				Log.Info($"Position: ({pos.X}, {pos.Y}, {pos.Z})");
			}

			if (Input.IsMouseButtonPressed(MouseCode.ButtonMiddle))
			{
				Vector2 curMousePos = Input.GetMousePosition();

				Log.Trace($"Current Mouse Pos: ({curMousePos.X}, {curMousePos.Y})");
			}
		}
	}
}

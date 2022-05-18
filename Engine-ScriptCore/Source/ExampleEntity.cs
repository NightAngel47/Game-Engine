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
			Log.Info($"Entity Created with ID: {ID}");

			TagComponent tagComponent = GetComponent<TagComponent>();
			string name = tagComponent.Tag;
			Log.Info($"Name: {name}");

			TransformComponent transformComponent = GetComponent<TransformComponent>();
			Vector3 pos = transformComponent.Position;
			Vector3 rot = transformComponent.Rotation;
			Vector3 scale = transformComponent.Scale;
			Log.Info($"Position: ({pos.x}, {pos.y}, {pos.z})");
			Log.Info($"Rotation: ({rot.x}, {rot.y}, {rot.z})");
			Log.Info($"Scale: ({scale.x}, {scale.y}, {scale.z})");
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
				Vector2 curMousePos = Input.GetMousePosition();

				Log.Trace($"Current Mouse Pos: ({curMousePos.x}, {curMousePos.y})");
			}
		}
	}
}

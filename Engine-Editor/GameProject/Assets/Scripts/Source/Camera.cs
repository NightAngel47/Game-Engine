using Engine.Scene;
using Engine.Core;
using Engine.Math;

namespace GameProject.Source
{
	public class Camera : Entity
	{
		public float DistanceFromPlayer = 5.0f;

		Entity player;

		protected override void OnCreate()
		{
			player = FindEntityByName("Player");
		}

		protected override void OnUpdate(float ts)
		{
			if (player != null)
			{
				Vector2 playerPos = player.Position;
				Position = new Vector3(playerPos.X, playerPos.Y, DistanceFromPlayer);
			}
			else
			{
				Log.Error($"Player is null!");
			}
		}
	}
}

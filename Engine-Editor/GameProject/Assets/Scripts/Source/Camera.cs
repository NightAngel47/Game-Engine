using Engine.Scene;
using Engine.Core;
using Engine.Math;

namespace GameProject.Source
{
	public class Camera : Entity
	{
		public float DistanceFromPlayer = 5.0f;
		public float DistanceFromPlayerMin = 5.0f;
		public float DistanceFromPlayerMax = 50.0f;

		public float zoomSpeedPercentIncrease = 1.0f;
		public Vector2 offSet = new(0.0f, -2.0f);

		Entity player;
		Rigidbody2DComponent playerRb2d;

		protected override void OnStart()
		{
			player ??= FindEntityByName("Player");
			playerRb2d = player.GetComponent<Rigidbody2DComponent>();
		}

		protected override void OnLateUpdate(float ts)
		{
			if (player != null)
			{
				Vector2 playerPos = player.Position; 
				Position = new Vector3(playerPos + offSet, MaintainDistanceFromPlayer(0.7f));
			}
			else
			{
				Log.Error($"Player is null!");
			}
		}

		float MaintainDistanceFromPlayer(float ts)
		{
			float playerVelocityY = playerRb2d.LinearVelocity.Y;
			if (playerVelocityY > 0.0f)
				DistanceFromPlayer = playerVelocityY * (1.0f + zoomSpeedPercentIncrease * ts);

			Mathf.Lerp(Position.Y + offSet.Y, DistanceFromPlayer, ts);
			return Mathf.Clamp(DistanceFromPlayer, DistanceFromPlayerMin, DistanceFromPlayerMax);
		}
	}
}

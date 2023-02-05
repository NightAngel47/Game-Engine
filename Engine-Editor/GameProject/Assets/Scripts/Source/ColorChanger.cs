using Engine.Core;
using Engine.Math;
using Engine.Scene;

namespace GameProject.Source
{
	public class ColorChanger : Entity
	{
		SpriteRendererComponent sprite;
		public float secondsBetween = 1.0f;

		public Vector4 color;
		
		float timeElapsed;

		protected override void OnCreate()
		{
			sprite = GetComponent<SpriteRendererComponent>();
		}

		protected override void OnUpdate(float ts)
		{
			timeElapsed += ts;
			if (timeElapsed >= secondsBetween)
			{
				color = new Vector4(Random.Float(), Random.Float(), Random.Float(), 1.0f);
				sprite.Color = color;
				timeElapsed = 0;
			}
		}
	}
}

﻿using Engine.Core;
using Engine.Math;
using Engine.Scene;

namespace GameProject.Source
{
	public class ColorChanger : Entity
	{
		SpriteRendererComponent sprite;
		public float secondsBetween = 1.0f;
		public int maxTilingAmount = 100;
		
		float timeElapsed;

		protected override void OnCreate()
		{
			sprite = GetComponent<SpriteRendererComponent>();
		}

		protected override void OnUpdate(Timestep ts)
		{
			timeElapsed += ts;
			if (timeElapsed >= secondsBetween)
			{
				sprite.Color = new Vector4(Random.Float(), Random.Float(), Random.Float(), 1.0f);
				sprite.Tiling = Random.Range(1, maxTilingAmount);
				timeElapsed = 0;
			}
		}
	}
}

using Engine.Math;
using Engine.Core;
using Engine.Physics;

namespace Engine.Scene
{
	public abstract class Component
	{
		public Entity Entity { get; internal set; } = null;
	}

	public class TransformComponent : Component
	{
		public Vector3 Position
		{
			get
			{
				InternalCalls.TransformComponent_GetPosition(Entity.ID, out Vector3 position);
				return position;
			}

			set => InternalCalls.TransformComponent_SetPosition(Entity.ID, ref value);
		}

		public Vector3 Rotation
		{
			get
			{
				InternalCalls.TransformComponent_GetRotation(Entity.ID, out Vector3 rotation);
				return rotation;
			}

			set => InternalCalls.TransformComponent_SetRotation(Entity.ID, ref value);
		}

		public Vector3 Scale
		{
			get
			{
				InternalCalls.TransformComponent_GetScale(Entity.ID, out Vector3 scale);
				return scale;
			}

			set => InternalCalls.TransformComponent_SetScale(Entity.ID, ref value);
		}
	}

	public class SpriteRendererComponent : Component
	{
		public Vector4 Color
		{
			get
			{
				InternalCalls.SpriteRendererComponent_GetColor(Entity.ID, out Vector4 color);
				return color;
			}

			set => InternalCalls.SpriteRendererComponent_SetColor(Entity.ID, ref value);
		}

		public float Tiling
		{
			get => InternalCalls.SpriteRendererComponent_GetTiling(Entity.ID);
			set => InternalCalls.SpriteRendererComponent_SetTiling(Entity.ID, value);
		}
	}

	public class CircleRendererComponent : Component
	{
		public Vector4 Color
		{
			get
			{
				InternalCalls.CircleRendererComponent_GetColor(Entity.ID, out Vector4 color);
				return color;
			}

			set => InternalCalls.CircleRendererComponent_SetColor(Entity.ID, ref value);
		}

		public float Thinkness
		{
			get => InternalCalls.CircleRendererComponent_GetThinkness(Entity.ID);
			set => InternalCalls.CircleRendererComponent_SetThinkness(Entity.ID, value);
		}

		public float Fade
		{
			get => InternalCalls.CircleRendererComponent_GetFade(Entity.ID);
			set => InternalCalls.CircleRendererComponent_SetFade(Entity.ID, value);
		}
	}

	public class TextRendererComponent : Component
	{
		public Vector4 Color
		{
			get
			{
				InternalCalls.TextRendererComponent_GetColor(Entity.ID, out Vector4 color);
				return color;
			}

			set => InternalCalls.TextRendererComponent_SetColor(Entity.ID, ref value);
		}

		public string Text
		{
			get => InternalCalls.TextRendererComponent_GetText(Entity.ID);
			set => InternalCalls.TextRendererComponent_SetText(Entity.ID, value);
		}

		public float Kerning
		{
			get => InternalCalls.TextRendererComponent_GetKerning(Entity.ID);
			set => InternalCalls.TextRendererComponent_SetKerning(Entity.ID, value);
		}

		public float LineSpacing
		{
			get => InternalCalls.TextRendererComponent_GetLineSpacing(Entity.ID);
			set => InternalCalls.TextRendererComponent_SetLineSpacing(Entity.ID, value);
		}
	}

	public class UIImageComponent : Component
	{
		public Vector4 Color
		{
			get
			{
				InternalCalls.UIImageComponent_GetColor(Entity.ID, out Vector4 color);
				return color;
			}

			set => InternalCalls.UIImageComponent_SetColor(Entity.ID, ref value);
		}

		public float Tiling
		{
			get => InternalCalls.UIImageComponent_GetTiling(Entity.ID);
			set => InternalCalls.UIImageComponent_SetTiling(Entity.ID, value);
		}
	}

	public class UICircleComponent : Component
	{
		public Vector4 Color
		{
			get
			{
				InternalCalls.UICircleComponent_GetColor(Entity.ID, out Vector4 color);
				return color;
			}

			set => InternalCalls.UICircleComponent_SetColor(Entity.ID, ref value);
		}

		public float Thinkness
		{
			get => InternalCalls.UICircleComponent_GetThinkness(Entity.ID);
			set => InternalCalls.UICircleComponent_SetThinkness(Entity.ID, value);
		}

		public float Fade
		{
			get => InternalCalls.UICircleComponent_GetFade(Entity.ID);
			set => InternalCalls.UICircleComponent_SetFade(Entity.ID, value);
		}
	}

	public class UITextComponent : Component
	{
		public Vector4 Color
		{
			get
			{
				InternalCalls.UITextComponent_GetColor(Entity.ID, out Vector4 color);
				return color;
			}

			set => InternalCalls.UITextComponent_SetColor(Entity.ID, ref value);
		}

		public string Text
		{
			get => InternalCalls.UITextComponent_GetText(Entity.ID);
			set => InternalCalls.UITextComponent_SetText(Entity.ID, value);
		}

		public float Kerning
		{
			get => InternalCalls.UITextComponent_GetKerning(Entity.ID);
			set => InternalCalls.UITextComponent_SetKerning(Entity.ID, value);
		}

		public float LineSpacing
		{
			get => InternalCalls.UITextComponent_GetLineSpacing(Entity.ID);
			set => InternalCalls.UITextComponent_SetLineSpacing(Entity.ID, value);
		}
	}

	public class Rigidbody2DComponent : Component
	{
		public Vector2 LinearVelocity
		{
			get
			{
				InternalCalls.Rigidbody2DComponent_GetLinearVelocity(Entity.ID, out Vector2 velocity);
				return velocity;
			}

			set => InternalCalls.Rigidbody2DComponent_SetLinearVelocity(Entity.ID, ref value);
		}

		public BodyType Type
		{
			get
			{
				InternalCalls.Rigidbody2DComponent_GetType(Entity.ID, out int bodyType);
				return (BodyType)bodyType;
			}

			set => InternalCalls.Rigidbody2DComponent_SetType(Entity.ID, (int)value);
		}

		public void ApplyLinearImpulse(Vector2 impulse, Vector2 worldPosition, bool wake)
		{
			InternalCalls.Rigidbody2DComponent_ApplyLinearImpulse(Entity.ID, ref impulse, ref worldPosition, wake);
		}

		public void ApplyLinearImpulse(Vector2 impulse, bool wake)
		{
			InternalCalls.Rigidbody2DComponent_ApplyLinearImpulseToCenter(Entity.ID, ref impulse, wake);
		}

		public void ApplyForce(Vector2 force, Vector2 worldPosition, bool wake)
		{
			InternalCalls.Rigidbody2DComponent_ApplyForce(Entity.ID, ref force, ref worldPosition, wake);
		}

		public void ApplyForce(Vector2 force, bool wake)
		{
			InternalCalls.Rigidbody2DComponent_ApplyForceToCenter(Entity.ID, ref force, wake);
		}
	}

	public class BoxCollider2DComponent : Component
	{

	}

	public class CircleCollider2DComponent : Component
	{

	}
}

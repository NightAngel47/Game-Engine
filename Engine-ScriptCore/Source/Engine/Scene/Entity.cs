namespace Engine.Scene
{
	public abstract class Entity
	{
		protected abstract void OnCreate();
		protected abstract void OnDestroy();
		protected abstract void OnUpdate(float ts);
	}
}

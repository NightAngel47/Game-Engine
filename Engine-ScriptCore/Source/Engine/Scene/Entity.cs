namespace Engine.Scene
{
	public abstract class Entity
	{
		public ulong ID { get; private set; }

		protected abstract void OnCreate();
		protected abstract void OnDestroy();
		protected abstract void OnUpdate(float ts);

		public abstract T GetComponent<T>(); // TODO fix
	}
}

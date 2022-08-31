#pragma once

//https://stackoverflow.com/questions/13980157/c-class-with-template-member-variable

namespace Engine
{
	class ScriptFieldDataBase
	{
	public:
		virtual ~ScriptFieldDataBase() {}
		template<class T> const T& get() const; //to be implemented after ScriptFieldDataBase
		template<class T> void setValue(const T& rhs); //to be implemented after ScriptFieldDataBase
	};
	
	template <typename T>
	class ScriptFieldData : public ScriptFieldDataBase
	{
	public:
		ScriptFieldData(const T& rhs) :value(rhs) {}
		const T& get() const { return value; }
		void setValue(const T& rhs) { value = rhs; }
	private:
		T value;
	};
}

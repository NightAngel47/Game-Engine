#include "enginepch.h"
#include "Engine/Scripting/ScriptFieldData.h"

namespace Engine
{
	template<class T> const T& ScriptFieldDataBase::get() const
	{
		return dynamic_cast<const ScriptFieldData<T>&>(*this).get();
	}

	template<> const float& ScriptFieldDataBase::get() const
	{
		return dynamic_cast<const ScriptFieldData<float>&>(*this).get();
	}

	template<> const int& ScriptFieldDataBase::get() const
	{
		return dynamic_cast<const ScriptFieldData<int>&>(*this).get();
	}

	template<> const std::string& ScriptFieldDataBase::get() const
	{
		return dynamic_cast<const ScriptFieldData<std::string>&>(*this).get();
	}

	template<class T> void ScriptFieldDataBase::setValue(const T& rhs)
	{
		return dynamic_cast<ScriptFieldData<T>&>(*this).setValue(rhs);
	}

	template<> void ScriptFieldDataBase::setValue(const float& rhs)
	{
		return dynamic_cast<ScriptFieldData<float>&>(*this).setValue(rhs);
	}

	template<> void ScriptFieldDataBase::setValue(const int& rhs)
	{
		return dynamic_cast<ScriptFieldData<int>&>(*this).setValue(rhs);
	}

	template<> void ScriptFieldDataBase::setValue(const std::string& rhs)
	{
		return dynamic_cast<ScriptFieldData<std::string>&>(*this).setValue(rhs);
	}
}

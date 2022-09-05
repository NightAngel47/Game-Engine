#include "enginepch.h"
#include "Engine/Scripting/ScriptFieldData.h"

#include <glm/glm.hpp>

namespace Engine
{
	template<class T> const T& ScriptFieldDataBase::get() const
	{
		return static_cast<const ScriptFieldData<T>&>(*this).get();
	}

	template<> const float& ScriptFieldDataBase::get() const
	{
		return static_cast<const ScriptFieldData<float>&>(*this).get();
	}

	template<> const int& ScriptFieldDataBase::get() const
	{
		return static_cast<const ScriptFieldData<int>&>(*this).get();
	}

	template<> const std::string& ScriptFieldDataBase::get() const
	{
		return static_cast<const ScriptFieldData<std::string>&>(*this).get();
	}

	template<> const glm::vec2& ScriptFieldDataBase::get() const
	{
		return static_cast<const ScriptFieldData<glm::vec2>&>(*this).get();
	}

	template<> const glm::vec3& ScriptFieldDataBase::get() const
	{
		return static_cast<const ScriptFieldData<glm::vec3>&>(*this).get();
	}

	template<> const glm::vec4& ScriptFieldDataBase::get() const
	{
		return static_cast<const ScriptFieldData<glm::vec4>&>(*this).get();
	}

	template<class T> void ScriptFieldDataBase::setValue(const T& rhs)
	{
		return static_cast<ScriptFieldData<T>&>(*this).setValue(rhs);
	}

	template<> void ScriptFieldDataBase::setValue(const float& rhs)
	{
		return static_cast<ScriptFieldData<float>&>(*this).setValue(rhs);
	}

	template<> void ScriptFieldDataBase::setValue(const int& rhs)
	{
		return static_cast<ScriptFieldData<int>&>(*this).setValue(rhs);
	}

	template<> void ScriptFieldDataBase::setValue(const std::string& rhs)
	{
		return static_cast<ScriptFieldData<std::string>&>(*this).setValue(rhs);
	}

	template<> void ScriptFieldDataBase::setValue(const glm::vec2& rhs)
	{
		return static_cast<ScriptFieldData<glm::vec2>&>(*this).setValue(rhs);
	}

	template<> void ScriptFieldDataBase::setValue(const glm::vec3& rhs)
	{
		return static_cast<ScriptFieldData<glm::vec3>&>(*this).setValue(rhs);
	}

	template<> void ScriptFieldDataBase::setValue(const glm::vec4& rhs)
	{
		return static_cast<ScriptFieldData<glm::vec4>&>(*this).setValue(rhs);
	}
}

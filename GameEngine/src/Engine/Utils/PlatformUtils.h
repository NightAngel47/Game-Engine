#pragma once

#include <string>

namespace Engine
{
	class FileDialogs
	{
	public:
		// There return empty string if cancelled
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);
	};

	class Time
	{
	public:
		static float GetTime();
	};
}

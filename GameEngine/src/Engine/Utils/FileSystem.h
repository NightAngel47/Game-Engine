#pragma once
#include "Engine/Core/Buffer.h"

namespace Engine
{
	class FileSystem
	{
	public:
		static Buffer ReadFileBinary(const std::filesystem::path& filepath);
	};
}


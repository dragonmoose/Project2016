#pragma once
#include "Macros.h"
#include <string>

namespace Hawk {
namespace LogSystem
{
	enum class Level
	{
		Info,
		Warning,
		Error
	};

	bool Initialize();

	HAWK_DLL_EXPORT void Write(const std::string& p_Msg, Level p_Level);
}
}
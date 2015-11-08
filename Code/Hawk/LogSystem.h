#pragma once
#include "Macros.h"
#include <string>

namespace Hawk {
namespace LogSystem
{
	enum class Type
	{
		Info,
		Warning,
		Error,
		Exception
	};

	bool Initialize();

	HAWK_DLL_EXPORT void Write(const std::string& p_Msg, Type p_Level);
}
}
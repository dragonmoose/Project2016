#pragma once
#include "System/DllExport.h"
#include <string>

namespace Hawk {
namespace FileUtil
{
	HAWK_DLL_EXPORT std::string GetFilenameWithoutPath(const std::string& p_Path);
}
}
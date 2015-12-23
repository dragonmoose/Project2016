#pragma once
#include "System/DllExport.h"
#include <string>

namespace Hawk {
namespace StringUtil
{
	HAWK_DLL_EXPORT bool AreEqual(const std::string& p_Str1, const std::string& p_Str2, bool p_bCaseSensitive = false);
	HAWK_DLL_EXPORT std::string ToLower(const std::string& p_Source);
	HAWK_DLL_EXPORT bool Contains(const std::string& p_Str, const std::string& p_SubStr);
}

}
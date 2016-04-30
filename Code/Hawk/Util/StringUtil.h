#pragma once
#include "System/DllExport.h"
#include <string>

namespace Hawk {
namespace StringUtil
{
	HAWK_DLL_EXPORT bool AreEqual(const std::string& p_Str1, const std::string& p_Str2, bool p_bCaseSensitive = false);
	HAWK_DLL_EXPORT std::string ToLower(const std::string& p_Source);
	HAWK_DLL_EXPORT std::string ToUpper(const std::string& p_Source);
	HAWK_DLL_EXPORT bool Contains(const std::string& p_Str, const std::string& p_SubStr, bool p_bCaseSensitive = false);
	HAWK_DLL_EXPORT bool StartsWith(const std::string& p_Str, const std::string& p_SubStr, bool p_bCaseSensitive = false);
	HAWK_DLL_EXPORT std::string RemoveBack(const std::string& p_Str, std::size_t p_Num);
	HAWK_DLL_EXPORT std::string WCharToString(const wchar_t* p_Str);
}

}
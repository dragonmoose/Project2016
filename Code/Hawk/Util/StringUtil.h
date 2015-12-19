#pragma once
#include <string>

namespace Hawk {
namespace StringUtil
{
	bool AreEqual(const std::string& p_Str1, const std::string& p_Str2, bool p_bCaseSensitive = false);
	std::string ToLower(const std::string& p_Source);
	bool Contains(const std::string& p_Str, const std::string& p_SubStr);
}

}
#include "pch.h"
#include "StringUtil.h"
#include "boost/algorithm/string.hpp"

namespace Hawk {

bool StringUtil::AreEqual(const std::string& p_Str1, const std::string& p_Str2, bool p_bCaseSensitive)
{
	return p_bCaseSensitive ? boost::iequals(p_Str1, p_Str2) : boost::iequals(p_Str1, p_Str2);
}

std::string StringUtil::ToLower(const std::string& p_Source)
{
	return boost::algorithm::to_lower_copy(p_Source);
}

bool StringUtil::Contains(const std::string& p_Str, const std::string& p_SubStr)
{
	return boost::algorithm::contains(p_Str, p_SubStr);
}

bool StringUtil::StartsWith(const std::string& p_Str, const std::string& p_SubStr, bool p_bCaseSensitive)
{
	return p_bCaseSensitive ? boost::starts_with(p_Str, p_SubStr) : boost::istarts_with(p_Str, p_SubStr);
}

std::string StringUtil::RemoveBack(const std::string& p_Str, size_t p_Num)
{
	if (p_Num <= p_Str.size())
	{
		return p_Str.substr(0, p_Str.size() - p_Num);
	}
	LOG("Attempting to remove more characters than size of string. String=" << p_Str << " NumChars=" << p_Num, "util", Error);
	return p_Str;
}

}
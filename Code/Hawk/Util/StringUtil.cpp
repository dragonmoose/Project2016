#include "pch.h"
#include "Util/StringUtil.h"
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

}
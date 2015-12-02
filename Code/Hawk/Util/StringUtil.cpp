#include "pch.h"
#include "Util/StringUtil.h"
#include "boost/algorithm/string.hpp"

namespace Hawk {

std::string StringUtil::ToLower(const std::string& p_Source)
{
	return boost::algorithm::to_lower_copy(p_Source);
}

bool StringUtil::Contains(const std::string& p_Str, const std::string& p_SubStr)
{
	return boost::algorithm::contains(p_Str, p_SubStr);
}

}
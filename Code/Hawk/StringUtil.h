#pragma once
#include <string>

namespace Hawk {
namespace StringUtil
{
	std::string ToLower(const std::string& p_Source);
	bool Contains(const std::string& p_Str, const std::string& p_SubStr);
}

}
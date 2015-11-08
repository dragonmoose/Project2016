#pragma once
#include <stdexcept>
#include "Macros.h"

namespace Hawk {

class HAWK_DLL_EXPORT Exception : public std::runtime_error
{
public:
	explicit Exception(const char* p_Msg) : std::runtime_error(p_Msg) {}
	explicit Exception(const std::string& p_Msg) : std::runtime_error(p_Msg) {}
};

}
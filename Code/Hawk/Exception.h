#pragma once
#include <stdexcept>
#include "Macros.h"

namespace Hawk {

class HAWK_DLL_EXPORT Exception : public std::runtime_error
{
public:
	explicit Exception(const std::string& p_Msg, const std::string& p_File, int p_iLine);
	const std::string& GetSourceInfo() const;
private:
	std::string m_SourceInfo;
};

}
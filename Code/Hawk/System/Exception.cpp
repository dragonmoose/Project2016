#include "pch.h"
#include "Exception.h"

namespace Hawk {

Exception::Exception(const std::string& p_Msg, const std::string& p_File, int p_iLine)
: std::runtime_error(p_Msg)
{
	m_SourceInfo = p_File + ":" + std::to_string(p_iLine);
}

const std::string& Exception::GetSourceInfo() const
{
	return m_SourceInfo;
}

}


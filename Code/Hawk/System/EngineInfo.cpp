#include "pch.h"
#include "EngineInfo.h"
#include <boost/lexical_cast.hpp>
#include <sstream>

namespace Hawk {

EngineInfo::EngineInfo(const std::string& p_Name, uint16_t p_uiMajor, uint8_t p_uiMinor, uint8_t p_uiPatch)
: m_Name(p_Name)
{
	m_Version.m_Components.m_uiMajor = boost::lexical_cast<uint16_t>(p_uiMajor);
	m_Version.m_Components.m_uiMinor = boost::lexical_cast<uint8_t>(p_uiMinor);
	m_Version.m_Components.m_uiPatch = boost::lexical_cast<uint8_t>(p_uiPatch);

	std::ostringstream l_Stream;
	l_Stream << p_uiMajor << "." << (int)p_uiMinor << "." << (int)p_uiPatch;
	m_VersionStr = l_Stream.str();
	m_NameAndVersionStr = m_Name + " " + m_VersionStr;
}

uint32_t EngineInfo::GetVersionID() const
{
	return m_Version.m_ID;
}

const std::string& EngineInfo::GetVersion() const
{
	return m_VersionStr;
}

const std::string& EngineInfo::GetName() const
{
	return m_Name;
}

const std::string& EngineInfo::GetNameAndVersion() const
{
	return m_NameAndVersionStr;
}

}
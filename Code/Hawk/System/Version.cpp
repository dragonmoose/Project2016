#include "pch.h"
#include "Version.h"
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <sstream>

namespace Hawk {

Version::Version(uint16_t p_uiMajor, uint8_t p_uiMinor, uint8_t p_uiPatch)
{
	m_Version.m_Parts.m_uiMajor = boost::lexical_cast<uint16_t>(p_uiMajor);
	m_Version.m_Parts.m_uiMinor = boost::lexical_cast<uint8_t>(p_uiMinor);
	m_Version.m_Parts.m_uiPatch = boost::lexical_cast<uint8_t>(p_uiPatch);

	std::ostringstream l_Stream;
	l_Stream << p_uiMajor << "." << (int)p_uiMinor << "." << (int)p_uiPatch;
	m_Str = l_Stream.str();

	LOG("Version: " << m_Str << " Value=" << m_Version.m_Value, "core", Info);
}

uint32_t Version::GetValue() const
{
	return m_Version.m_Value;
}

const std::string& Version::GetString() const
{
	return m_Str;
}
}
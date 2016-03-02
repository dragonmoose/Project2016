#include "pch.h"
#include "Version.h"
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <sstream>

namespace Hawk {

Version::Version(uint16_t p_uiMajor, uint8_t p_uiMinor, uint8_t p_uiPatch)
{
	m_Value.m_Parts.m_uiMajor = boost::lexical_cast<uint16_t>(p_uiMajor);
	m_Value.m_Parts.m_uiMinor = boost::lexical_cast<uint8_t>(p_uiMinor);
	m_Value.m_Parts.m_uiPatch = boost::lexical_cast<uint8_t>(p_uiPatch);

	std::ostringstream l_Stream;
	l_Stream << p_uiMajor << "." << (int)p_uiMinor << "." << (int)p_uiPatch;
	m_Str = l_Stream.str();
}

uint32_t Version::GetID() const
{
	return m_Value.m_ID;
}

const std::string& Version::GetString() const
{
	return m_Str;
}

bool Version::IsValid() const
{
	return GetID() > 0;
}
}
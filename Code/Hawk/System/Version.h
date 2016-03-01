#pragma once
#include <string>

namespace Hawk {

class Version final
{
public:
	Version(uint16_t p_uiMajor, uint8_t p_uiMinor, uint8_t p_uiPatch);
	uint32_t GetValue() const;
	const std::string& GetString() const;
private:
	union
	{
		struct
		{
			uint16_t m_uiMajor;
			uint8_t m_uiMinor;
			uint8_t m_uiPatch;
		} m_Parts;
		uint32_t m_Value;
	} m_Version;
	std::string m_Str;
};

}
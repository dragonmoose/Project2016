#pragma once
#include <string>

namespace Hawk {

class HAWK_DLL_EXPORT Version final
{
public:
	Version(uint16_t p_uiMajor, uint8_t p_uiMinor, uint8_t p_uiPatch);
	uint32_t GetID() const;
	const std::string& GetString() const;
	bool IsValid() const;
private:
	union
	{
		struct
		{
			uint16_t m_uiMajor;
			uint8_t m_uiMinor;
			uint8_t m_uiPatch;
		} m_Parts;
		uint32_t m_ID;
	} m_Value;
	std::string m_Str;
};

}
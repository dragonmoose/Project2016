#pragma once
#include <string>

namespace Hawk {

class EngineInfo final
{
public:
	EngineInfo(const std::string& p_Name, uint16_t p_uiMajor, uint8_t p_uiMinor, uint8_t p_uiPatch);

	uint32_t GetVersionID() const;
	const std::string& GetVersion() const;
	const std::string& GetName() const;
	const std::string& GetNameAndVersion() const;

private:
	union
	{
		struct
		{
			uint16_t m_uiMajor;
			uint8_t m_uiMinor;
			uint8_t m_uiPatch;
		} m_Components;
		uint32_t m_ID;
	} m_Version;

	std::string m_Name;
	std::string m_VersionStr;
	std::string m_NameAndVersionStr;
};

}
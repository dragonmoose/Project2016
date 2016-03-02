#include "pch.h"
#include "CoreInfo.h"

namespace Hawk {

namespace CoreInfo
{
	std::string m_EngineName("Hawk");
	Version m_EngineVersion(1, 0, 0);

	std::string m_AppName;
	Version m_AppVersion(0, 0, 0);
}

const char* CoreInfo::GetEngineName()
{
	return m_EngineName.c_str();
}
const Version& CoreInfo::GetEngineVersion()
{
	return m_EngineVersion;
}

const char* CoreInfo::GetAppName()
{
	return m_AppName.c_str();
}
const Version& CoreInfo::GetAppVersion()
{
	return m_AppVersion;
}

void CoreInfo::_InitApp(const std::string& p_Name, const Version& p_Version)
{
	m_AppName = p_Name;
	m_AppVersion = p_Version;
}

}
#pragma once
#include "System/Version.h"
#include <windef.h>
#include <string>

namespace Hawk {

struct CoreSettings final
{
	CoreSettings()
	: m_bConsole(true)
	, m_ConfigFilename("config.ini")
	, m_bRenderingModule(true)
	, m_hInstance(nullptr)
	, m_AppVersion(0, 0, 0)
	{}

	bool m_bConsole;
	std::string m_ConfigFilename;
	bool m_bRenderingModule;
	HINSTANCE m_hInstance;
	std::string m_AppName;
	Version m_AppVersion;
};
}
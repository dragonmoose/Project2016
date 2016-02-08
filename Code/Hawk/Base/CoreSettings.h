#pragma once
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
	{}

	bool m_bConsole;
	std::string m_ConfigFilename;
	bool m_bRenderingModule;
	HINSTANCE m_hInstance;
	std::string m_WindowName;
};
}
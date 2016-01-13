#pragma once
#include <string>

namespace Hawk {

struct CoreSettings final
{
	CoreSettings()
	: m_bConsole(true)
	, m_ConfigFilename("config.ini")
	, m_bRenderingModule(true)
	{}

	bool m_bConsole;
	std::string m_ConfigFilename;
	bool m_bRenderingModule;
};
}
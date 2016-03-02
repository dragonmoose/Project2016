#pragma once
#include <cstdint>
#include "Version.h"

namespace Hawk {
namespace CoreInfo
{
	const char* GetEngineName();
	const Version& GetEngineVersion();

	const char* GetAppName();
	const Version& GetAppVersion();

	void _InitApp(const std::string& p_Name, const Version& p_Version);
}
}
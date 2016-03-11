#pragma once
#include <cstdint>
#include "DllExport.h"
#include "Version.h"

namespace Hawk {
namespace CoreInfo
{
	HAWK_DLL_EXPORT const char* GetEngineName();
	HAWK_DLL_EXPORT const Version& GetEngineVersion();

	HAWK_DLL_EXPORT const char* GetAppName();
	HAWK_DLL_EXPORT const Version& GetAppVersion();

	void _InitApp(const std::string& p_Name, const Version& p_Version);
}
}
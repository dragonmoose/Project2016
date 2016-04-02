#pragma once
#include "System/DllExport.h"
#include <string>
#include <ctime>

namespace Hawk {
namespace FileUtil
{
	HAWK_DLL_EXPORT std::string GetFilenameWithoutPath(const std::string& p_Path);
	HAWK_DLL_EXPORT bool HasExtension(const std::string& p_Path, const std::string& p_Ext);
	HAWK_DLL_EXPORT bool Exists(const std::string& p_Path);
	HAWK_DLL_EXPORT uintmax FileSize(const std::string& p_Path);
	HAWK_DLL_EXPORT std::time_t LastWriteTime(const std::string& p_Path);

}
}
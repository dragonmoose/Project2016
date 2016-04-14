#include "pch.h"
#include "FileUtil.h"
#include "StringUtil.h"
#include <boost/filesystem.hpp>

namespace Hawk {

std::string FileUtil::GetFilenameWithoutPath(const std::string& p_Path)
{
	return boost::filesystem::path(p_Path).filename().string();
}

bool FileUtil::HasExtension(const std::string& p_Path, const std::string& p_Ext)
{
	return StringUtil::AreEqual(boost::filesystem::path(p_Path).extension().string(), p_Ext);
}

bool FileUtil::Exists(const std::string& p_Path)
{
	return boost::filesystem::exists(p_Path);
}

uintmax FileUtil::FileSize(const std::string& p_Path)
{
	return boost::filesystem::file_size(p_Path);
}

std::time_t FileUtil::LastWriteTime(const std::string& p_Path)
{
	return boost::filesystem::last_write_time(p_Path);
}

}
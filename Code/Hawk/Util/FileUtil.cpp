#include "pch.h"
#include "Util/FileUtil.h"
#include <boost/filesystem/path.hpp>

namespace Hawk {

std::string FileUtil::GetFilenameWithoutPath(const std::string& p_Path)
{
	return boost::filesystem::path(p_Path).filename().string();
}
}
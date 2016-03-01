#pragma once
#include "System/Types.h"
#include <string>
#include <unordered_map>
#include <thread>

namespace Hawk {
namespace ThreadInfoManager
{
	void RegisterThread(const std::string& p_Name, std::thread::id p_SysThreadID, ThreadID_t p_ID = ThreadID_Invalid);
	bool IsThread(const std::string& p_Name);
	const std::string& GetLogText(std::thread::id p_SysThreadID);
}
}
#include "pch.h"
#include "ThreadInfoManager.h"
#include "Mutex.h"
#include "Debug/Assert.h"

namespace Hawk {
namespace ThreadInfoManager
{
	std::string CreateLogText(const std::string& p_Name, std::thread::id p_SysThreadID, ThreadID p_ID);
	
	using LogTextMap_t = std::unordered_map<std::thread::id, std::string>;
	LogTextMap_t n_LogTextMap;
	Mutex n_LogTextMutex("ThreadLogText");

	using SysIDMap_t = std::unordered_map<std::string, std::thread::id>;
	SysIDMap_t n_SysIDMap;
	Mutex n_SysIDMutex("ThreadSysID");

	const std::string c_NotAvailable("N/A");
}

std::string ThreadInfoManager::CreateLogText(const std::string& p_Name, std::thread::id p_SysThreadID, ThreadID p_ID)
{
	std::ostringstream l_Stream;
	l_Stream << p_Name;

	if (p_ID != ThreadID_Invalid)
	{
		l_Stream << " #" << p_ID;
	}
	l_Stream << " [" << p_SysThreadID << "]";
	return l_Stream.str();
}

void ThreadInfoManager::RegisterThread(const std::string& p_Name, std::thread::id p_SysThreadID, ThreadID p_ID)
{
	ASSERT(p_SysThreadID != std::thread::id(), "Invalid system thread id");
	{
		MutexScope_t l_MutexScope(n_LogTextMutex);
		THROW_IF_NOT(n_LogTextMap.insert(LogTextMap_t::value_type(p_SysThreadID, CreateLogText(p_Name, p_SysThreadID, p_ID))).second, "Failed to insert thread log text entry. Name=" << p_Name << " ID=" << p_ID);
	}
	{
		MutexScope_t l_MutexScope(n_SysIDMutex);
		THROW_IF_NOT(n_SysIDMap.insert(SysIDMap_t::value_type(p_Name, p_SysThreadID)).second, "Failed to insert system thread entry. Name=" << p_Name << " ID=" << p_ID);
	}

}

bool ThreadInfoManager::IsThread(const std::string& p_Name)
{
	MutexScope_t l_MutexScope(n_SysIDMutex);
	auto l_Itr = n_SysIDMap.find(p_Name);
	return l_Itr != n_SysIDMap.end() ? (l_Itr->second == std::this_thread::get_id()) : false;
}

const std::string& ThreadInfoManager::GetLogText(std::thread::id p_SysThreadID)
{
	MutexScope_t l_MutexScope(n_LogTextMutex);
	auto l_Itr = n_LogTextMap.find(p_SysThreadID);
	return l_Itr != n_LogTextMap.end() ? l_Itr->second : c_NotAvailable;
}

}
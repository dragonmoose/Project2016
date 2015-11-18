#include "pch.h"

#ifdef HAWK_DEBUG

#include "Logger.h"
#include "Time.h"
#include "Duration.h"
#include "boost/algorithm/string.hpp"
#include <unordered_map>
#include <consoleapi.h>
#include <io.h>
#include <fcntl.h>
#include <mutex>
#include <cctype>

namespace Hawk {

namespace Logger
{
	WORD GetConsoleTextAttr(Level p_Level);
	bool ShouldLog(const std::string& p_Msg, const std::string& p_ThreadInfo);
	std::string GetThreadInfo();

	std::mutex n_Mutex;
	bool n_bInitialized = false;
	typedef std::unordered_map<std::thread::id, std::string> ThreadNames_t;
	ThreadNames_t n_ThreadNames;
}

bool Logger::Initialize()
{
	if (n_bInitialized) return true;

	// Follows: https://justcheckingonall.wordpress.com/2008/08/29/console-window-win32-app/
	if (AllocConsole())
	{
		{
			HANDLE l_hStd = GetStdHandle(STD_OUTPUT_HANDLE);
			int l_hCrt = _open_osfhandle((intptr_t)l_hStd, _O_TEXT);
			FILE* l_hFile = _fdopen(l_hCrt, "w");
			setvbuf(l_hFile, nullptr, _IONBF, 1);
			*stdout = *l_hFile;
		}
		{
			HANDLE l_hStd = GetStdHandle(STD_INPUT_HANDLE);
			int l_hCrt = _open_osfhandle((intptr_t)l_hStd, _O_TEXT);
			FILE* l_hFile = _fdopen(l_hCrt, "r");
			setvbuf(l_hFile, nullptr, _IONBF, 128);
			*stdin = *l_hFile;
		}
		SetConsoleTitle("Hawk Engine console");
		n_bInitialized = true;
		return true;
	}
	return false;
}

void Logger::RegisterThreadName(const std::string& p_Name, std::thread::id p_ID)
{
	// Should only be called from main thread
	THROW_IF_NOT(n_ThreadNames.insert(ThreadNames_t::value_type(p_ID, p_Name)).second, "Failed to insert thread name lookup. Name=" << p_Name << " ID=" << p_ID);
}

void Logger::Write(const std::string& p_Msg, Level p_Level)
{
	std::string l_ThreadInfo = GetThreadInfo();
	if (!ShouldLog(p_Msg, l_ThreadInfo)) return;

	std::lock_guard<std::mutex> l_Lock(n_Mutex);
	if (p_Level != Level::Debug || (Config::Instance().Get("Log.debug", false)))
	{
		HANDLE l_hStd = GetStdHandle(STD_OUTPUT_HANDLE);

		SetConsoleTextAttribute(l_hStd, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

		std::ostringstream l_Stream;

		l_Stream << Time::Now() << " " << l_ThreadInfo << "\t";
		std::string l_InitStr = l_Stream.str();
		WriteConsole(l_hStd, l_InitStr.c_str(), l_InitStr.length(), LPDWORD(), nullptr);

		SetConsoleTextAttribute(l_hStd, GetConsoleTextAttr(p_Level));
		WriteConsole(l_hStd, p_Msg.c_str(), p_Msg.length(), LPDWORD(), nullptr);
	}
}

WORD Logger::GetConsoleTextAttr(Level p_Level)
{
	switch (p_Level)
	{
		case Level::Debug:
			return FOREGROUND_RED | FOREGROUND_BLUE;
		case Level::Info:
			return FOREGROUND_GREEN;
		case Level::Warning:
			return FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN;
		case Level::Error:
			return FOREGROUND_INTENSITY | FOREGROUND_RED;
		case Level::Exception:
			return FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_INTENSITY | BACKGROUND_RED;
		default:
			return 0;
	}
}

bool Logger::ShouldLog(const std::string& p_Msg, const std::string& p_ThreadInfo)
{
	if (!Config::Instance().Get<bool>("Log.enabled", true)) return false;
	std::string l_Filter = Config::Instance().Get<std::string>("Log.filter", "");
	std::string l_ThreadFilter = Config::Instance().Get<std::string>("Log.thread", "");

	bool l_bLog = true;
	if (!l_Filter.empty())
	{
		boost::algorithm::to_lower(l_Filter);
		std::string l_Msg(p_Msg);
		boost::algorithm::to_lower(l_Msg);

		l_bLog = boost::algorithm::contains(l_Msg, l_Filter);
	}
	if (l_bLog && !l_ThreadFilter.empty())
	{
		boost::algorithm::to_lower(l_ThreadFilter);
		std::string l_ThreadInfo(p_ThreadInfo);
		boost::algorithm::to_lower(l_ThreadInfo);

		l_bLog = boost::algorithm::contains(l_ThreadInfo, l_ThreadFilter);
	}
	return l_bLog;
}

std::string Logger::GetThreadInfo()
{
	std::thread::id l_ID = std::this_thread::get_id();
	std::ostringstream l_Stream;
	l_Stream << "[";
	auto l_Itr = n_ThreadNames.find(l_ID);
	if (l_Itr != n_ThreadNames.end())
	{
		l_Stream << l_Itr->second;
	}
	else
	{
		l_Stream << "N/A";
	}
	l_Stream << " #" << l_ID << "]";
	return l_Stream.str();
}

}

#endif
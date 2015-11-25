#include "pch.h"
#include <atomic>

namespace Hawk {
namespace Logger
{
	std::atomic<bool> m_bFatalFlag(false);
	void SetFatalFlag() { m_bFatalFlag = true; }
	bool FatalFlagSet() { return m_bFatalFlag; }
}
}

#ifdef HAWK_DEBUG
#include "Logger.h"
#include "Time.h"
#include "Duration.h"
#include "HawkVersion.h"
#include "boost/algorithm/string.hpp"
#include <unordered_map>
#include <consoleapi.h>
#include <io.h>
#include <fcntl.h>
#include <mutex>
#include <cctype>
#include <chrono>

namespace Hawk {
namespace Logger
{
	WORD GetMsgTextAttr(Level p_Level);
	bool ShouldLog(const std::string& p_Msg, const std::string& p_ThreadInfo, const std::string& p_System);
	std::string GetThreadInfo();
	void ClearScreen();
	void SetFont();

	const WORD c_SysInfoTextAttr = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_BLUE;
	const WORD c_FileInfoTextAttr = FOREGROUND_BLUE | FOREGROUND_RED;
	std::mutex n_Mutex;
	bool n_bInitialized = false;
	using ThreadNames_t = std::unordered_map<std::thread::id, std::string>;
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
			SetConsoleMode(l_hStd, ENABLE_QUICK_EDIT_MODE);
		}
		std::ostringstream l_TitleStream;
		l_TitleStream << "Hawk Engine Console " << Hawk::Version::c_EngineVersion << " [" << __DATE__ << "]";
		SetConsoleTitle(l_TitleStream.str().c_str());
		SetFont();
		n_bInitialized = true;
		return true;
	}
	return false;
}

void Logger::RegisterThreadName(const std::string& p_Name, std::thread::id p_ID)
{
	std::lock_guard<std::mutex> l_Lock(n_Mutex);
	THROW_IF_NOT(n_ThreadNames.insert(ThreadNames_t::value_type(p_ID, p_Name)).second, "Failed to insert thread name lookup. Name=" << p_Name << " ID=" << p_ID);
}

void Logger::Write(const std::string& p_Msg, const std::string& p_System, const std::string& p_FileInfo, Level p_Level)
{
	std::string l_ThreadInfo = GetThreadInfo();
	if (!ShouldLog(p_Msg, l_ThreadInfo, p_System)) return;

	std::lock_guard<std::mutex> l_Lock(n_Mutex);
	if (p_Level != Level::Debug || (Config::Instance().Get("Log.debug", false)))
	{
		HANDLE l_hStd = GetStdHandle(STD_OUTPUT_HANDLE);

		SetConsoleTextAttribute(l_hStd, c_SysInfoTextAttr);

		std::ostringstream l_Stream;
		l_Stream << Time::Now() << " [" << l_ThreadInfo << " : " << p_System << "]";
		std::string l_InitStr = l_Stream.str();
		WriteConsole(l_hStd, l_InitStr.c_str(), l_InitStr.length(), LPDWORD(), nullptr);

		SetConsoleTextAttribute(l_hStd, 0);
		WriteConsole(l_hStd, " ", 1, LPDWORD(), nullptr);

		SetConsoleTextAttribute(l_hStd, GetMsgTextAttr(p_Level));
		WriteConsole(l_hStd, p_Msg.c_str(), p_Msg.length(), LPDWORD(), nullptr);

		SetConsoleTextAttribute(l_hStd, 0);
		WriteConsole(l_hStd, " ", 1, LPDWORD(), nullptr);

		SetConsoleTextAttribute(l_hStd, c_FileInfoTextAttr);
		WriteConsole(l_hStd, p_FileInfo.c_str(), p_FileInfo.length(), LPDWORD(), nullptr);
	}
}

WORD Logger::GetMsgTextAttr(Level p_Level)
{
	switch (p_Level)
	{
		case Level::Debug:
			return FOREGROUND_GREEN;
		case Level::Info:
			return FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
		case Level::Warning:
			return FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN;
		case Level::Error:
			return FOREGROUND_INTENSITY | FOREGROUND_RED;
		case Level::Fatal:
			return FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_INTENSITY | BACKGROUND_RED;
		default:
			return 0;
	}
}

bool Logger::ShouldLog(const std::string& p_Msg, const std::string& p_ThreadInfo, const std::string& p_System)
{
	if (!Config::Instance().Get<bool>("Log.enabled", true)) return false;
	std::string l_Filter = Config::Instance().Get<std::string>("Log.filter", "");
	std::string l_ThreadFilter = Config::Instance().Get<std::string>("Log.thread", "");
	std::string l_SystemFilter = Config::Instance().Get<std::string>("Log.system", "");

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
	if (l_bLog && !l_SystemFilter.empty())
	{
		boost::algorithm::to_lower(l_SystemFilter);
		std::string l_System(p_System);
		boost::algorithm::to_lower(l_System);
		l_bLog = boost::algorithm::contains(l_System, l_SystemFilter);
	}
	return l_bLog;
}

std::string Logger::GetThreadInfo()
{
	std::lock_guard<std::mutex> l_Lock(n_Mutex);
	std::thread::id l_ID = std::this_thread::get_id();
	std::ostringstream l_Stream;
	auto l_Itr = n_ThreadNames.find(l_ID);
	if (l_Itr != n_ThreadNames.end())
	{
		l_Stream << l_Itr->second;
	}
	else
	{
		l_Stream << "N/A";
	}
	l_Stream << " #" << l_ID;
	return l_Stream.str();
}

void Logger::ClearScreen()
{
	// Based on: https://support.microsoft.com/sv-se/kb/99261
	HANDLE l_hStd = GetStdHandle(STD_OUTPUT_HANDLE);

	COORD l_StartPos = { 0, 0 };
	CONSOLE_SCREEN_BUFFER_INFO l_ScreenBufferInfo;
	DWORD l_dwNumCharsInBuffer;
	DWORD l_dwCharsWritten;

	GetConsoleScreenBufferInfo(l_hStd, &l_ScreenBufferInfo);
	l_dwNumCharsInBuffer = l_ScreenBufferInfo.dwSize.X * l_ScreenBufferInfo.dwSize.Y;

	FillConsoleOutputCharacter(l_hStd, (TCHAR) ' ', l_dwNumCharsInBuffer, l_StartPos, &l_dwCharsWritten);
	GetConsoleScreenBufferInfo(l_hStd, &l_ScreenBufferInfo);
	FillConsoleOutputAttribute(l_hStd, l_ScreenBufferInfo.wAttributes, l_dwNumCharsInBuffer, l_StartPos, &l_dwCharsWritten);

	SetConsoleCursorPosition(l_hStd, l_StartPos);
}

void Logger::SetFont()
{
	CONSOLE_FONT_INFOEX l_FontInfo;
	l_FontInfo.cbSize = sizeof(l_FontInfo);
	l_FontInfo.nFont = 0;
	l_FontInfo.dwFontSize.X = 0;
	l_FontInfo.dwFontSize.Y = 13;
	l_FontInfo.FontFamily = FF_DONTCARE;
	l_FontInfo.FontWeight = FW_NORMAL;
	wcscpy_s(l_FontInfo.FaceName, L"Consolas");
	SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &l_FontInfo);
}
}
#endif
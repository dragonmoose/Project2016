#include "pch.h"

#ifdef HAWK_DEBUG

#include "Logger.h"
#include "Time.h"
#include <consoleapi.h>
#include <io.h>
#include <fcntl.h>
#include <mutex>

namespace Hawk {

namespace Logger
{
	WORD GetConsoleTextAttr(Level p_Level);
	std::mutex m_Mutex;
	bool m_bInitialized = false;
}

bool Logger::Initialize()
{
	if (m_bInitialized) return true;

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
		LOG_INFO("Logger initialized")
		m_bInitialized = true;
		return true;
	}
	return false;
}

void Logger::Write(const std::string& p_Msg, Level p_Level)
{
	std::lock_guard<std::mutex> l_Lock(m_Mutex);
	if (p_Level != Level::Debug || (Config::Get("Log.debug", false)))
	{
		HANDLE l_hStd = GetStdHandle(STD_OUTPUT_HANDLE);

		SetConsoleTextAttribute(l_hStd, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

		std::stringstream l_Stream;
		l_Stream << Time().ToString() << " (Thread: " << std::this_thread::get_id() << ") ";
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

}

#endif
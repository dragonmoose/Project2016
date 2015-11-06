#include "pch.h"
#include "LogSystem.h"
#include "Time.h"
#include <iostream>
#include <sstream>
#include <consoleapi.h>
#include <io.h>
#include <fcntl.h>

namespace Hawk {

namespace LogSystem
{
	WORD GetConsoleTextAttr(LogSystem::Level p_Level);
}

bool LogSystem::Initialize()
{
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
		return true;
	}
	return false;
}

void LogSystem::Write(const std::string& p_Msg, Level p_Level)
{
	std::stringstream l_Stream;
	l_Stream << Time::Now().ToString() << " " << p_Msg;
	std::string l_Msg(l_Stream.str());

	HANDLE l_hStd = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(l_hStd, GetConsoleTextAttr(p_Level));
	WriteConsole(l_hStd, l_Msg.c_str(), l_Msg.length(), LPDWORD(), nullptr);
}

WORD LogSystem::GetConsoleTextAttr(Level p_Level)
{
	switch (p_Level)
	{
		case Level::Info:
			return FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
		case Level::Warning:
			return FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN;
		case Level::Error:
			return FOREGROUND_INTENSITY | FOREGROUND_RED;
		default:
			return 0;
	}
}

}

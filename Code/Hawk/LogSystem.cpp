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
	WORD GetConsoleTextAttr(LogSystem::Type p_Level);
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
		SetConsoleTitle("Hawk Engine console");
		return true;
	}
	return false;
}

void LogSystem::Write(const std::string& p_Msg, Type p_Type)
{
	HANDLE l_hStd = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(l_hStd, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	std::string l_TimeStr = Time().ToString() + " ";
	WriteConsole(l_hStd, l_TimeStr.c_str(), l_TimeStr.length(), LPDWORD(), nullptr);

	SetConsoleTextAttribute(l_hStd, GetConsoleTextAttr(p_Type));
	WriteConsole(l_hStd, p_Msg.c_str(), p_Msg.length(), LPDWORD(), nullptr);

	std::stringstream l_Stream;
	l_Stream << " [" << __FILE__ << ":" << __LINE__ << "]\r\n";
	std::string l_LineInfoStr = l_Stream.str();

	WriteConsole(l_hStd, l_LineInfoStr.c_str(), l_LineInfoStr.length(), LPDWORD(), nullptr);
}

WORD LogSystem::GetConsoleTextAttr(Type p_Type)
{
	switch (p_Type)
	{
		case Type::Info:
			return FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
		case Type::Warning:
			return FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN;
		case Type::Error:
			return FOREGROUND_INTENSITY | FOREGROUND_RED;
		case Type::Exception:
			return FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | BACKGROUND_INTENSITY | BACKGROUND_RED;
		default:
			return 0;
	}
}

}

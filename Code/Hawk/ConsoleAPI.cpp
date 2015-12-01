#include "pch.h"
#include "ConsoleAPI.h"
#include "Exception.h"
#include "Version.h"
#include <consoleapi.h>
#include <mutex>

namespace Hawk {

namespace ConsoleAPI
{
	void SetTitle();
	void SetFont();
	WORD GetColorAttr(Color p_Color, Color p_BgColor);

	const COORD c_BufferSize = { 200, 1000 };
	HANDLE n_hOut;
	HANDLE n_hIn;
	std::mutex n_Mutex;
}

void ConsoleAPI::SetTitle()
{
	std::ostringstream l_TitleStream;
	l_TitleStream << "Hawk Engine Console " << Hawk::Version::c_EngineVersion << " [" << __DATE__ << "]";
	THROW_IF_NOT(::SetConsoleTitle(l_TitleStream.str().c_str()), "Failed to set console title");
}

void ConsoleAPI::SetFont()
{
	::CONSOLE_FONT_INFOEX l_FontInfo;
	l_FontInfo.cbSize = sizeof(l_FontInfo);
	l_FontInfo.nFont = 0;
	l_FontInfo.dwFontSize.X = 0;
	l_FontInfo.dwFontSize.Y = 13;
	l_FontInfo.FontFamily = FF_DONTCARE;
	l_FontInfo.FontWeight = FW_NORMAL;
	wcscpy_s(l_FontInfo.FaceName, L"Consolas");
	THROW_IF_NOT(::SetCurrentConsoleFontEx(n_hOut, FALSE, &l_FontInfo), "Failed to set font");
}

WORD ConsoleAPI::GetColorAttr(Color p_Color, Color p_BgColor)
{
	WORD l_wFinalColor = 0;
	switch (p_Color)
	{
		case Color::None:
			break;
		case Color::White:
			l_wFinalColor |= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
			break;
		case Color::BrightWhite:
			l_wFinalColor |= FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
			break;
		case Color::Red:
			l_wFinalColor |= FOREGROUND_RED;
			break;
		case Color::BrightRed:
			l_wFinalColor |= FOREGROUND_INTENSITY | FOREGROUND_RED;
			break;
		case Color::Green:
			l_wFinalColor |= FOREGROUND_GREEN;
			break;
		case Color::BrightGreen:
			l_wFinalColor |= FOREGROUND_INTENSITY | FOREGROUND_GREEN;
			break;
		case Color::Blue:
			l_wFinalColor |= FOREGROUND_BLUE;
			break;
		case Color::BrightBlue:
			l_wFinalColor |= FOREGROUND_INTENSITY | FOREGROUND_BLUE;
			break;
		case Color::Yellow:
			l_wFinalColor |= FOREGROUND_RED | FOREGROUND_GREEN;
			break;
		case Color::BrightYellow:
			l_wFinalColor |= FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN;
			break;
		case Color::Magenta:
			l_wFinalColor |= FOREGROUND_RED | FOREGROUND_BLUE;
			break;
		case Color::BrightMagenta:
			l_wFinalColor |= FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE;
			break;
		case Color::Cyan:
			l_wFinalColor |= FOREGROUND_GREEN | FOREGROUND_BLUE;
			break;
		case Color::BrightCyan:
			l_wFinalColor |= FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE;
			break;
		default:
			THROW("Invalid foreground color");
	}

	switch (p_BgColor)
	{
		case Color::None:
			break;
		case Color::White:
			l_wFinalColor |= BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
			break;
		case Color::BrightWhite:
			l_wFinalColor |= BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
			break;
		case Color::Red:
			l_wFinalColor |= BACKGROUND_RED;
			break;
		case Color::BrightRed:
			l_wFinalColor |= BACKGROUND_INTENSITY | BACKGROUND_RED;
			break;
		case Color::Green:
			l_wFinalColor |= BACKGROUND_GREEN;
			break;
		case Color::BrightGreen:
			l_wFinalColor |= BACKGROUND_INTENSITY | BACKGROUND_GREEN;
			break;
		case Color::Blue:
			l_wFinalColor |= BACKGROUND_BLUE;
			break;
		case Color::BrightBlue:
			l_wFinalColor |= BACKGROUND_INTENSITY | BACKGROUND_BLUE;
			break;
		case Color::Yellow:
			l_wFinalColor |= BACKGROUND_RED | BACKGROUND_GREEN;
			break;
		case Color::BrightYellow:
			l_wFinalColor |= BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN;
			break;
		case Color::Magenta:
			l_wFinalColor |= BACKGROUND_RED | BACKGROUND_BLUE;
			break;
		case Color::BrightMagenta:
			l_wFinalColor |= BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE;
			break;
		case Color::Cyan:
			l_wFinalColor |= BACKGROUND_GREEN | BACKGROUND_BLUE;
			break;
		case Color::BrightCyan:
			l_wFinalColor |= BACKGROUND_INTENSITY | BACKGROUND_GREEN | BACKGROUND_BLUE;
			break;
		default:
			THROW("Invalid background color");
	}
	return l_wFinalColor;
}

void ConsoleAPI::Start()
{
	THROW_IF_NOT(AllocConsole(), "Failed to allocate console");

	n_hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	THROW_IF(n_hOut == INVALID_HANDLE_VALUE, "Invalid output handle");

	n_hIn = GetStdHandle(STD_INPUT_HANDLE);
	THROW_IF(n_hIn == INVALID_HANDLE_VALUE, "Invalid input handle");
	
	THROW_IF_NOT(::SetConsoleMode(n_hIn, ENABLE_QUICK_EDIT_MODE | ENABLE_EXTENDED_FLAGS), "Failed to set console mode");
	THROW_IF_NOT(::SetConsoleScreenBufferSize(n_hOut, c_BufferSize), "Failed to set console buffer size");

	CONSOLE_CURSOR_INFO l_CursorInfo;
	l_CursorInfo.dwSize = 100;
	l_CursorInfo.bVisible = true;
	THROW_IF_NOT(::SetConsoleCursorInfo(n_hOut, &l_CursorInfo), "Failed to set console cursor info");

	SetTitle();
	SetFont();
}

void ConsoleAPI::WriteLine(const std::string& p_Text, Color p_Color, Color p_BgColor)
{
	std::string l_Text(p_Text);
	l_Text += '\n';
	Write(l_Text, p_Color, p_BgColor);
}

void ConsoleAPI::Write(const std::string& p_Text, Color p_Color, Color p_BgColor)
{
	std::lock_guard<std::mutex> l_Lock(n_Mutex);
	THROW_IF_NOT(::SetConsoleTextAttribute(n_hOut, GetColorAttr(p_Color, p_BgColor)), "Failed to set text attribute");
	THROW_IF_NOT(::WriteConsole(n_hOut, p_Text.c_str(), p_Text.length(), LPDWORD(), nullptr), "Failed to write to console");
}

bool ConsoleAPI::HasNextChar()
{
	DWORD l_dwNumEvents = 0;
	THROW_IF_NOT(::GetNumberOfConsoleInputEvents(n_hIn, &l_dwNumEvents), "GetNumberOfConsoleInputEvents failed");
	return l_dwNumEvents > 0;
}

bool ConsoleAPI::TryGetNextChar(char& p_cChr)
{
	INPUT_RECORD l_InputRecord;
	DWORD l_dwNumEventsRead = 0;
	THROW_IF_NOT(::ReadConsoleInput(n_hIn, &l_InputRecord, 1, &l_dwNumEventsRead), "ReadConsoleInput failed");

	if (l_InputRecord.EventType == KEY_EVENT && l_InputRecord.Event.KeyEvent.bKeyDown)
	{
		p_cChr = l_InputRecord.Event.KeyEvent.uChar.AsciiChar;
		return true;
	}
	return false;
}

void ConsoleAPI::ClearScreen()
{
	// Based on: https://support.microsoft.com/sv-se/kb/99261
	COORD l_StartPos = { 0, 0 };
	CONSOLE_SCREEN_BUFFER_INFO l_ScreenBufferInfo;
	DWORD l_dwNumCharsInBuffer;
	DWORD l_dwCharsWritten;

	THROW_IF_NOT(::GetConsoleScreenBufferInfo(n_hOut, &l_ScreenBufferInfo), "Failed to get console screen buffer info");
	l_dwNumCharsInBuffer = l_ScreenBufferInfo.dwSize.X * l_ScreenBufferInfo.dwSize.Y;

	THROW_IF_NOT(::FillConsoleOutputCharacter(n_hOut, (TCHAR) ' ', l_dwNumCharsInBuffer, l_StartPos, &l_dwCharsWritten), "Failed to fill console");
	THROW_IF_NOT(::GetConsoleScreenBufferInfo(n_hOut, &l_ScreenBufferInfo), "Failed to get console screen buffer info");
	THROW_IF_NOT(::FillConsoleOutputAttribute(n_hOut, l_ScreenBufferInfo.wAttributes, l_dwNumCharsInBuffer, l_StartPos, &l_dwCharsWritten), "Failed to fill console");
	THROW_IF_NOT(::SetConsoleCursorPosition(n_hOut, l_StartPos), "Failed to set console position");
}

void ConsoleAPI::Stop()
{
	THROW_IF_NOT(::FreeConsole(), "Failed to free console");
}

}
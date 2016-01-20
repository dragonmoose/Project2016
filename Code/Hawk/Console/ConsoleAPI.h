#pragma once

#include <string>

namespace Hawk {
namespace ConsoleAPI
{
	enum class Color
	{
		None,
		White,
		BrightWhite,
		Red,
		BrightRed,
		Green,
		BrightGreen,
		Blue,
		BrightBlue,
		Yellow,
		BrightYellow,
		Magenta,
		BrightMagenta,
		Cyan,
		BrightCyan
	};

	void Start();
	void BeginWrite();
	void EndWrite();
	void WriteLine(const std::string& p_Text, Color p_Color, Color p_BgColor);
	void Write(const std::string& p_Text, Color p_Color, Color p_BgColor);
	bool HasNextChar();
	bool TryGetNextChar(char& p_cChar, bool& p_bVirtualKeyCode);
	void ClearScreen();
	void ClearCurrLine();
	void Stop();
	bool Initialized();

	class WriteScope final
	{
	public:
		WriteScope() { BeginWrite(); }
		~WriteScope() { EndWrite(); }

		WriteScope(const WriteScope&) = delete;
		WriteScope& operator=(const WriteScope&) = delete;
	};
}
}

#define CONSOLE_WRITE_SCOPE()			ConsoleAPI::WriteScope _WriteScope_;
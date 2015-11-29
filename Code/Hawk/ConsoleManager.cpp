#include "pch.h"
#include "ConsoleManager.h"
#include <consoleapi.h>

namespace Hawk {

ConsoleManager::ConsoleManager()
: m_bStopSignal(false)
{

}

void ConsoleManager::Start()
{
	m_Thread = std::thread(&ConsoleManager::Run_Thread, this);
}

void ConsoleManager::Stop()
{
	m_bStopSignal = true;
	m_Thread.join();
}

void ConsoleManager::Run_Thread()
{
	LOG("Console command thread started", "console", Info);
	HANDLE l_hIn = GetStdHandle(STD_INPUT_HANDLE);
	
	char l_Char[1] = {0};
	DWORD l_dwNumCharsRead = 0;
	SetConsoleMode(l_hIn, 0);

		while (!m_bStopSignal)
	{
		ReadConsole(l_hIn, l_Char, 1, &l_dwNumCharsRead, nullptr);
		if (l_Char[0] == 13)
		{
			Logger::WriteCmd("\n");
		}
		else
		{
			Logger::WriteCmd(&l_Char[0]);
		}
	}
}
}
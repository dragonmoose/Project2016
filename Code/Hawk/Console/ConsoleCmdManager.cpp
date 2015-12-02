#include "pch.h"
#include "Console/ConsoleCmdManager.h"
#include "Console/ConsoleAPI.h"

namespace Hawk {

ConsoleCmdManager::ConsoleCmdManager()
{
}

void ConsoleCmdManager::Update()
{
	ConsoleInputHandler::Commands_t l_Commands;
	if (m_InputHandler.TryPopQueued(l_Commands))
	{
		for (auto l_Command : l_Commands)
		{
			if (l_Command == "cls")
			{
				ConsoleAPI::ClearScreen();
			}
			else if (l_Command == "quit")
			{
				::PostQuitMessage(0);
			}
		}
	}
}

void ConsoleCmdManager::Start()
{
	m_InputHandler.Start();
}

void ConsoleCmdManager::Stop()
{
	m_InputHandler.Stop();
}
}
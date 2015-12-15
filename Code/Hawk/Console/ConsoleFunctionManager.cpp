#include "pch.h"
#include "Console/ConsoleFunctionManager.h"
#include "Console/ConsoleAPI.h"
#include "Console/ConsoleCommand.h"

namespace Hawk {

ConsoleFunctionManager::ConsoleFunctionManager()
: m_Router(std::make_shared<ConsoleFunctionRouter>())
{
	/*Register("cls", this, &ConsoleFunctionManager::CmdCls);
	Register("quit", this, &ConsoleFunctionManager::CmdQuit);
	Register("cont", this, &ConsoleFunctionManager::CmdContinueAfterInterrupt);*/
}

void ConsoleFunctionManager::Start()
{
	m_InputHandler.Start();
}

void ConsoleFunctionManager::Stop()
{
	m_InputHandler.Stop();
}

void ConsoleFunctionManager::Update()
{
	ConsoleInputHandler::InputLines_t l_InputLines;
	if (m_InputHandler.TryPopQueued(l_InputLines))
	{
		for (auto l_InputLine : l_InputLines)
		{
			try
			{
				ConsoleCommand l_Command(l_InputLine);
				std::shared_ptr<ModuleConsoleManager> m_Manager;
				if (m_Router->TryGetManager(l_Command.GetName(), m_Manager))
				{
					m_Manager->PushCommand(l_Command);
				}
			}
			catch (Exception& e)
			{
				LOG_EXCEPTION(e, "console", Error);
			}
		}
	}
}

void ConsoleFunctionManager::CmdCls()
{
	ConsoleAPI::ClearScreen();
}

void ConsoleFunctionManager::CmdQuit()
{
	::PostQuitMessage(0);
}

void ConsoleFunctionManager::CmdContinueAfterInterrupt()
{
	Logger::ContinueAfterInterrupt();
}


}
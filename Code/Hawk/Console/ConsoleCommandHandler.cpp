#include "pch.h"
#ifdef HAWK_DEBUG
#include "Console/ConsoleCommandHandler.h"
#include "Console/ConsoleAPI.h"
#include "Console/ConsoleCommand.h"

namespace Hawk {

ConsoleCommandHandler::ConsoleCommandHandler()
: m_ModuleRouter(std::make_shared<ModuleConsoleRouter>())
{
	/*Register("cls", this, &ConsoleCommandHandler::CmdCls);
	Register("quit", this, &ConsoleCommandHandler::CmdQuit);
	Register("cont", this, &ConsoleCommandHandler::CmdContinueAfterInterrupt);*/
}

void ConsoleCommandHandler::Start()
{
	m_InputHandler.Start();
}

void ConsoleCommandHandler::Stop()
{
	m_InputHandler.Stop();
}

void ConsoleCommandHandler::Update()
{
	ConsoleInputHandler::InputLines_t l_InputLines;
	if (m_InputHandler.TryPopQueued(l_InputLines))
	{
		for (auto l_InputLine : l_InputLines)
		{
			try
			{
				m_ModuleRouter->Dispatch(ConsoleCommand(l_InputLine));
			}
			catch (Exception& e)
			{
				LOG_EXCEPTION(e, "console", Error);
			}
		}
	}
}

std::shared_ptr<ModuleConsoleRouter> ConsoleCommandHandler::GetModuleRouter() const
{
	return m_ModuleRouter;
}

void ConsoleCommandHandler::CmdCls()
{
	//ConsoleAPI::ClearScreen();
}

void ConsoleCommandHandler::CmdQuit()
{
	//::PostQuitMessage(0);
}

void ConsoleCommandHandler::CmdContinueAfterInterrupt()
{
	//Logger::ContinueAfterInterrupt();
}

}
#endif
#include "pch.h"
#include "Console/ConsoleInputHandler.h"
#include "Console/ConsoleAPI.h"
#include "System/Version.h"

namespace Hawk {

namespace
{
	const char* c_Name("Console");
	const char Key_Return = 13;
	const char Key_Backspace = 8;
}

ConsoleInputHandler::ConsoleInputHandler()
: m_bStopSignal(false)
{

}

void ConsoleInputHandler::Start()
{
	m_Thread = std::thread(&ConsoleInputHandler::RunInputLoop, this);
}

void ConsoleInputHandler::Stop()
{
	m_bStopSignal = true;
	m_Thread.join();
}

bool ConsoleInputHandler::TryPopQueued(ConsoleInputHandler::Commands_t& p_Commands)
{
	std::lock_guard<std::mutex> l_Lock(m_Mutex);
	m_Commands.swap(p_Commands);
	return !p_Commands.empty();
}

void ConsoleInputHandler::RunInputLoop()
{
	LOG("Console command thread started", c_Name, Info);

	std::string l_CurrCmd;
	while (!m_bStopSignal)
	{
		while (ConsoleAPI::HasNextChar())
		{
			char l_cChr = 0;
			if (ConsoleAPI::TryGetNextChar(l_cChr))
			{
				if (l_cChr == Key_Backspace)
				{
					// backspace
				}
				else if (l_cChr == Key_Return)
				{
					{
						std::lock_guard<std::mutex> l_Lock(m_Mutex);
						m_Commands.push_back(l_CurrCmd);
					}
					std::stringstream l_CmdLine;
					l_CmdLine << "Hawk " << Version::c_EngineVersion << ">" << l_CurrCmd;
					ConsoleAPI::WriteLine(l_CmdLine.str(), ConsoleAPI::Color::White, ConsoleAPI::Color::None);
					l_CurrCmd.clear();
				}
				else
				{
					l_CurrCmd += l_cChr;
				}
			}
		}
		std::stringstream l_CmdLine;
		l_CmdLine << "\rHawk " << Version::c_EngineVersion << ">" << l_CurrCmd << "\r";
		ConsoleAPI::Write(l_CmdLine.str(), ConsoleAPI::Color::White, ConsoleAPI::Color::None);
		std::this_thread::sleep_for(std::chrono::milliseconds(25));
	}
}
}
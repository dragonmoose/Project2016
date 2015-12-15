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

bool ConsoleInputHandler::TryPopQueued(ConsoleInputHandler::InputLines_t& p_InputLines)
{
	std::lock_guard<std::mutex> l_Lock(m_Mutex);
	m_InputLines.swap(p_InputLines);
	return !p_InputLines.empty();
}

void ConsoleInputHandler::RunInputLoop()
{
	LOG("Console command thread started", c_Name, Info);

	std::string l_CurrLine;
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
						m_InputLines.push_back(l_CurrLine);
					}
					std::stringstream l_CmdLine;
					l_CmdLine << "Hawk " << Version::c_EngineVersion << ">" << l_CurrLine;
					ConsoleAPI::BeginWrite();
					ConsoleAPI::WriteLine(l_CmdLine.str(), ConsoleAPI::Color::White, ConsoleAPI::Color::None);
					ConsoleAPI::EndWrite();
					l_CurrLine.clear();
				}
				else
				{
					l_CurrLine += l_cChr;
				}
			}
		}
		std::stringstream l_CmdLine;
		l_CmdLine << "\rHawk " << Version::c_EngineVersion << ">" << l_CurrLine << "\r";
		ConsoleAPI::BeginWrite();
		ConsoleAPI::Write(l_CmdLine.str(), ConsoleAPI::Color::White, ConsoleAPI::Color::None);
		ConsoleAPI::EndWrite();
		std::this_thread::sleep_for(std::chrono::milliseconds(25));
	}
}
}
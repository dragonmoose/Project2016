#include "pch.h"
#ifdef HAWK_DEBUG
#include "Console/ConsoleCommandManager.h"
#include "Console/ConsoleCommand.h"
#include "Console/ConsoleAPI.h"
#include "System/Version.h"

namespace Hawk {

namespace
{
	const char* c_Name("Console");
	const char Key_Return = 13;
	const char Key_Backspace = 8;
}

ConsoleCommandManager::ConsoleCommandManager()
: m_bStopSignal(false)
{

}

void ConsoleCommandManager::Start()
{
	m_Thread = std::thread(&ConsoleCommandManager::RunInputLoop, this);
}

void ConsoleCommandManager::Stop()
{
	m_bStopSignal = true;
	m_Thread.join();
}

void ConsoleCommandManager::RunInputLoop()
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
						if (l_CurrLine.find_first_not_of("\t\n ") != std::string::npos)
						{
							try
							{
								ConsoleCommand l_Command(l_CurrLine);

								std::lock_guard<std::mutex> l_Lock(m_Mutex);
								auto l_Itr = m_Functions.find(l_Command.GetName());
								if (l_Itr != m_Functions.end())
								{
									l_Itr->second->_Call(l_Command.GetArgs());
								}
								else
								{
									LOG("Unknown command: " << l_Command.GetName(), "console", Info);
								}
							}
							catch (Exception& e)
							{
								LOG_EXCEPTION(e, "console", Fatal);
							}
						}
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
#endif
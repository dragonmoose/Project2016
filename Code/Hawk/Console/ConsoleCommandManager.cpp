#include "pch.h"
#ifdef HAWK_DEBUG
#include "Console/ConsoleCommandManager.h"
#include "Console/ConsoleCommand.h"
#include "Console/ConsoleAPI.h"
#include "System/Version.h"
#include "Util/StringUtil.h"

namespace Hawk {

namespace
{
	const char* c_Name("Console");
	const char Key_Shift = 75;
	const char Key_Return = 13;
	const char Key_Backspace = 8;
	
	void CmdQuit() { ::PostQuitMessage(-1); }
	void CmdCls() { ConsoleAPI::ClearScreen(); }
}

ConsoleCommandManager::ConsoleCommandManager(std::shared_ptr<Dispatcher>& p_Dispatcher)
: m_Dispatcher(p_Dispatcher)
, m_bStopSignal(false)
{
	Register("quit", &CmdQuit, m_Dispatcher.get());
	Register("cls", &CmdCls, m_Dispatcher.get());
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
	LOG("Console input thread started", c_Name, Info);
	Logger::RegisterThread("Thread_Console_Input", std::this_thread::get_id());

	std::string l_CurrLine;
	while (!m_bStopSignal)
	{
		try
		{
			while (ConsoleAPI::HasNextChar())
			{
				char l_cChr = 0;
				bool l_bModifierKey = false;
				if (ConsoleAPI::TryGetNextChar(l_cChr, l_bModifierKey))
				{
					if (!l_bModifierKey)
					{
						if (l_cChr == Key_Backspace)
						{
							if (l_CurrLine.size() > 0)
							{
								l_CurrLine = StringUtil::RemoveBack(l_CurrLine, 1);
								ConsoleAPI::ClearCurrLine();
							}
						}
						else if (l_cChr == Key_Return)
						{
							if (l_CurrLine.find_first_not_of("\t\n ") != std::string::npos)
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
			}
			std::stringstream l_CmdLineStream;
			l_CmdLineStream << "\rHawk " << Version::c_EngineVersion << ">" << l_CurrLine << "\r";
			std::string l_Line = l_CmdLineStream.str();
			ConsoleAPI::BeginWrite();
			ConsoleAPI::Write(l_Line, ConsoleAPI::Color::White, ConsoleAPI::Color::None);
			ConsoleAPI::EndWrite();
			std::this_thread::sleep_for(std::chrono::milliseconds(25));
		}
		catch (Exception& e)
		{
			LOG_EXCEPTION(e, "console", Fatal);
		}
	}
}

void ConsoleCommandManager::Unregister(const std::string& p_Name)
{
	std::string l_Name = StringUtil::ToLower(p_Name);
	auto l_Itr = m_Functions.find(l_Name);
	if (l_Itr != m_Functions.end())
	{
		m_Functions.erase(l_Itr);
	}
	else
	{
		LOG("Failed to unregister - command not found: " << l_Name, "console", Error);
	}
}
}
#endif
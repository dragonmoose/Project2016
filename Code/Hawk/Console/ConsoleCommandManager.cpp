#include "pch.h"
#ifdef HAWK_DEBUG
#include "Console/ConsoleCommandManager.h"
#include "Console/ConsoleCommand.h"
#include "Console/ConsoleAPI.h"
#include "System/Version.h"
#include "Util/StringUtil.h"
#include <algorithm>
#include <iomanip>

namespace Hawk {

namespace 
{
	const char* c_Name("Console");
	const char Key_Shift = 75;
	const char Key_Return = 13;
	const char Key_Backspace = 8;
}

ConsoleCommandManager::ConsoleCommandManager(std::shared_ptr<Dispatcher>& p_Dispatcher)
: m_Dispatcher(p_Dispatcher)
, m_bStopSignal(false)
{
}

void ConsoleCommandManager::Start()
{
	Register();
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
							std::stringstream l_CmdLine;
							l_CmdLine << "Hawk " << Version::c_EngineVersion << ">" << l_CurrLine;
							ConsoleAPI::BeginWrite();
							ConsoleAPI::WriteLine(l_CmdLine.str(), ConsoleAPI::Color::White, ConsoleAPI::Color::None);
							ConsoleAPI::EndWrite();

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

void ConsoleCommandManager::Register()
{
	Register("quit", this, &ConsoleCommandManager::CmdQuit, m_Dispatcher.get(), "Quits the application");
	Register("help", this, &ConsoleCommandManager::CmdListCommands, m_Dispatcher.get(), "Lists available commands. Args: [filter]");
	Register("cls", &ConsoleAPI::ClearScreen, m_Dispatcher.get(), "Clears the console buffer");
	Register("config.get", this, &ConsoleCommandManager::CmdGetConfig, m_Dispatcher.get(), "Gets the config variable. Args: [key]");
	Register("config.set", &Config::Instance(), &Config::Set, m_Dispatcher.get(), "Sets the config variable. Args: [key] [value]");
	Register("-", this, &ConsoleCommandManager::CmdToggleLog, m_Dispatcher.get(), "Toggles console log on/off");
}

void ConsoleCommandManager::CmdQuit()
{
	::PostQuitMessage(-1);
}

void ConsoleCommandManager::CmdListCommands(const std::string& p_Filter)
{
	CONSOLE_WRITE_SCOPE();
	std::string l_Filter = StringUtil::ToLower(p_Filter);
	std::cout << "----------------------- Available commands -----------------------\n";
	
	for (const auto& l_Cmd : m_Functions)
	{
		if (l_Filter.empty() || StringUtil::Contains(l_Cmd.first, l_Filter))
		{
			std::cout << std::setw(35) << std::left << l_Cmd.first << l_Cmd.second->GetHelpText() << "\n";
		}
	}
	std::cout << "------------------------------------------------------------------\n";
}

void ConsoleCommandManager::CmdGetConfig(const std::string& p_Key)
{
	CONSOLE_WRITE_SCOPE();
	std::cout << Config::Instance().Get<std::string>(p_Key, "Config key not found") << "\n";
}

void ConsoleCommandManager::CmdToggleLog()
{
	bool l_bCurrValue = Config::Instance().Get("log.enabled", true);
	std::string l_NewValue = l_bCurrValue ? "false" : "true";
	Config::Instance().Set("log.enabled", l_NewValue);
}

}
#endif
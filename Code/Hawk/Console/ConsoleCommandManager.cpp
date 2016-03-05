#include "pch.h"
#ifdef HAWK_DEBUG
#include "ConsoleCommandManager.h"
#include "ConsoleInputParser.h"
#include "System/Version.h"
#include "Util/Algorithm.h"
#include "Util/StringUtil.h"
#include "Threading/ThreadInfoManager.h"
#include <iomanip>

namespace Hawk {

namespace 
{
	const char Key_Shift = 75;
	const char Key_Return = 13;
	const char Key_Backspace = 8;
	const char Key_Tab = 9;

	const UINT32 c_uiMaxHistoryRecords = 30;
	const std::array<std::string, 5> c_HistoryExcludedCmds = { "-", "quit", "help", "cls", "console.history" };

	bool SaveToHistory(const std::string& p_Cmd)
	{
		return std::find(c_HistoryExcludedCmds.cbegin(), c_HistoryExcludedCmds.cend(), p_Cmd) == c_HistoryExcludedCmds.end();
	}
}

ConsoleCommandManager::ConsoleCommandManager(std::shared_ptr<Dispatcher>& p_Dispatcher)
: m_Dispatcher(p_Dispatcher)
, m_History(std::make_unique<ConsoleHistory>(c_uiMaxHistoryRecords))
, m_Thread("Console", std::bind(&ConsoleCommandManager::RunInputLoop, this))
{
	std::ostringstream l_Stream;
	l_Stream << CoreInfo::GetEngineName() << " " << CoreInfo::GetEngineVersion().GetString() << ">";
	m_Prompt = l_Stream.str();
}

void ConsoleCommandManager::Start()
{
	Register();
	m_Thread.Start();
}

void ConsoleCommandManager::Stop()
{
	m_Thread.Stop();
	m_History.reset(nullptr);
}

void ConsoleCommandManager::RunInputLoop()
{
	try
	{
		while (ConsoleAPI::HasNextChar())
		{
			char l_cChr = 0;
			bool l_bIsVirtualKeyCode = false;
			if (ConsoleAPI::TryGetNextChar(l_cChr, l_bIsVirtualKeyCode))
			{
				if (l_bIsVirtualKeyCode)
				{
					if (l_cChr == VK_UP)
					{
						if (m_History->Back())
						{
							ConsoleAPI::ClearCurrLine();
							m_CurrLine = m_History->GetCurrRecord();
						}
					}
					else if (l_cChr == VK_DOWN)
					{
						if (m_History->Forward())
						{
							ConsoleAPI::ClearCurrLine();
							m_CurrLine = m_History->GetCurrRecord();
						}
					}
				}
				else
				{
					if (l_cChr == Key_Backspace)
					{
						if (m_CurrLine.size() > 0)
						{
							m_CurrLine = StringUtil::RemoveBack(m_CurrLine, 1);
							ConsoleAPI::ClearCurrLine();
						}
						m_CurrTypedLine = m_CurrLine;
					}
					else if (l_cChr == Key_Tab)
					{
						ConsoleAPI::ClearCurrLine();
						m_CurrLine = GetNextCommand(m_CurrTypedLine, m_CurrLine);
					}
					else if (l_cChr == Key_Return)
					{
						std::stringstream l_CmdLine;
						l_CmdLine << m_Prompt << m_CurrLine;
						ConsoleAPI::BeginWrite();
						ConsoleAPI::WriteLine(l_CmdLine.str(), ConsoleAPI::Color::White, ConsoleAPI::Color::None);
						ConsoleAPI::EndWrite();

						if (m_CurrLine.find_first_not_of("\t\n ") != std::string::npos)
						{
							ConsoleInputParser l_ParsedInput(m_CurrLine);

							MutexScope_t l_MutexScope(m_Mutex);
							auto l_Itr = m_Functions.find(l_ParsedInput.GetCommand());
							if (l_Itr != m_Functions.end())
							{
								TryCallFunction(*l_Itr->second, l_ParsedInput);
							}
							else
							{
								std::cout << "Unknown command: " << l_ParsedInput.GetCommand() << "\n\n";
							}
							if (SaveToHistory(l_ParsedInput.GetCommand()))
							{
								m_History->Add(m_CurrLine);
							}
						}
						m_CurrLine.clear();
						m_CurrTypedLine.clear();
						std::this_thread::sleep_for(std::chrono::milliseconds(50));
					}
					else
					{
						m_CurrLine += l_cChr;
						m_CurrTypedLine = m_CurrLine;
					}
				}
			}
		}
		std::stringstream l_CmdLineStream;
		l_CmdLineStream << "\r" << m_Prompt << m_CurrLine << "\r";
		std::string l_Line = l_CmdLineStream.str();
		ConsoleAPI::BeginWrite();
		ConsoleAPI::Write(l_Line, ConsoleAPI::Color::White, ConsoleAPI::Color::None);
		ConsoleAPI::EndWrite();
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
	catch (Exception& e)
	{
		FATAL_EXCEPTION(e, "console");
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
	Register("quit", this, &ConsoleCommandManager::CmdQuit, m_Dispatcher.get(), "Quits the application", "");
	Register("help", this, &ConsoleCommandManager::CmdListCommands, m_Dispatcher.get(), "Lists available commands.", "[filter]", false);
	Register("cls", &ConsoleAPI::ClearScreen, m_Dispatcher.get(), "Clears the console buffer", "");
	Register("config.get", this, &ConsoleCommandManager::CmdGetConfig, m_Dispatcher.get(), "Gets the config variable.", "[key]");
	Register("config.set", &Config::Instance(), &Config::Set, m_Dispatcher.get(), "Sets the config variable.", "[key] [value]");
	Register("config.reload", this, &ConsoleCommandManager::CmdReloadConfig, m_Dispatcher.get(), "Reloads the config.ini file", "");
	Register("-", this, &ConsoleCommandManager::CmdToggleLog, m_Dispatcher.get(), "Toggles console log on/off", "");
	Register("log.level", this, &ConsoleCommandManager::CmdSetLogLevel, m_Dispatcher.get(), "Sets the loglevel", "[trace|debug|info|warning|error|fatal]");
	Register("log.filter", this, &ConsoleCommandManager::CmdSetLogFilter, m_Dispatcher.get(), "Sets the log filter", "[filter]", false);
	Register("log.thread", this, &ConsoleCommandManager::CmdSetLogThread, m_Dispatcher.get(), "Sets the thread filter", "[filter]", false);
	Register("log.tag", this, &ConsoleCommandManager::CmdSetLogTag, m_Dispatcher.get(), "Sets the tag filter", "[filter]", false);
	Register("console.history", this, &ConsoleCommandManager::CmdPrintHistory, m_Dispatcher.get(), "Prints the console history", "");
	Register("console.clearHistory", this, &ConsoleCommandManager::CmdClearHistory, m_Dispatcher.get(), "Clears the console history", "");
	Register("about", this, &ConsoleCommandManager::CmdAbout, m_Dispatcher.get(), "Displays engine info", "");
}

void ConsoleCommandManager::CmdQuit()
{
	::PostQuitMessage(-1);
}

void ConsoleCommandManager::CmdListCommands(const std::string& p_Filter)
{
	CONSOLE_WRITE_SCOPE();
	std::cout << "-------------------------------------------------------------------------------------------------\n";
	
	for (const auto& l_Cmd : m_Functions)
	{
		if (p_Filter.empty() || StringUtil::Contains(l_Cmd.first, p_Filter))
		{
			std::cout << std::setw(35) << std::left << l_Cmd.first << l_Cmd.second->GetDesc();
			if (!l_Cmd.second->GetArgsDesc().empty())
			{
				std::cout << " Args: " << l_Cmd.second->GetArgsDesc();
			}
			std::cout << "\n";
		}
	}
	std::cout << "-------------------------------------------------------------------------------------------------\n";
}

void ConsoleCommandManager::CmdGetConfig(const std::string& p_Key)
{
	CONSOLE_WRITE_SCOPE();
	std::cout << Config::Instance().Get<std::string>(p_Key, "Config key not found.") << "\n\n";
}

void ConsoleCommandManager::CmdReloadConfig()
{
	if (Config::Instance().Load(true))
	{
		std::cout << "Config reloaded.\n\n";
	}
	else
	{
		std::cout << "Failed to parse config file.\n\n";
	}

}

void ConsoleCommandManager::CmdToggleLog()
{
	bool l_bCurrValue = Config::Instance().Get("log.enabled", true);
	std::string l_NewValue = l_bCurrValue ? "false" : "true";
	Config::Instance().Set("log.enabled", l_NewValue);
}

void ConsoleCommandManager::CmdSetLogLevel(const std::string& p_Level)
{
	if (Logger::IsValidLogLevelString(p_Level))
	{
		Config::Instance().Set("log.level", p_Level);
	}
	else
	{
		CONSOLE_WRITE_SCOPE();
		std::cout << "Invalid log level.\n\n";
	}
}

void ConsoleCommandManager::CmdSetLogFilter(const std::string& p_Filter)
{
	Config::Instance().Set("log.filter", p_Filter);
}

void ConsoleCommandManager::CmdSetLogThread(const std::string& p_Filter)
{
	Config::Instance().Set("log.thread", p_Filter);
}

void ConsoleCommandManager::CmdSetLogTag(const std::string& p_Filter)
{
	Config::Instance().Set("log.tag", p_Filter);
}

void ConsoleCommandManager::CmdPrintHistory()
{
	CONSOLE_WRITE_SCOPE();
	m_History->Print();
}

void ConsoleCommandManager::CmdClearHistory()
{
	m_History->Clear();
	std::cout << "History cleared.\n\n";
}

void ConsoleCommandManager::CmdAbout()
{
	CONSOLE_WRITE_SCOPE();
	std::cout << "Engine name:\t\t" << CoreInfo::GetEngineName() << "\n";
	std::cout << "Engine version:\t\t" << CoreInfo::GetEngineVersion().GetString() << "\n";
	std::cout << "Engine version ID:\t\t" << CoreInfo::GetEngineVersion().GetID() << "\n";

	std::cout << "\nApp name:\t\t" << CoreInfo::GetAppName() << "\n";
	std::cout << "App version:\t\t" << CoreInfo::GetAppVersion().GetString() << "\n";
	std::cout << "App version ID:\t\t" << CoreInfo::GetAppVersion().GetID() << "\n";
}

std::string ConsoleCommandManager::GetNextCommand(const std::string& p_Filter, const std::string& p_Current) const
{
	if (p_Filter.empty())
	{
		if (p_Current.empty())
		{
			return m_Functions.cbegin()->first;
		}
		else
		{
			auto l_Itr = m_Functions.find(p_Current);
			if (l_Itr == m_Functions.cend())
			{
				return p_Current;
			}

			l_Itr++;
			if (l_Itr == m_Functions.cend())
			{
				return m_Functions.cbegin()->first;
			}
			return l_Itr->first;
		}
	}
	else
	{
		const std::string l_Filter = StringUtil::ToLower(p_Filter);
		using MatchVec_t = std::vector<std::string>;
		MatchVec_t l_Matches;
		hwk::transform_first_if(m_Functions, std::back_inserter(l_Matches), [l_Filter](const FunctionMap_t::value_type& p_Value)
		{
			return StringUtil::StartsWith(p_Value.first, l_Filter);
		});
		if (!l_Matches.empty())
		{
			const std::string l_Current = StringUtil::ToLower(p_Current);
			auto l_Itr = std::find(l_Matches.cbegin(), l_Matches.cend(), l_Current);
			if (l_Itr == l_Matches.cend())
			{
				return *l_Matches.cbegin();
			}

			l_Itr++;
			if (l_Itr == l_Matches.cend())
			{
				return *l_Matches.cbegin();
			}
			return *l_Itr;
		}
		return p_Current;
	}
}

void ConsoleCommandManager::TryCallFunction(const CF::IConsoleFunction& p_Function, ConsoleInputParser& p_Command) const
{
	if (!p_Function.RequiresArgs())
	{
		p_Command.TryExtendArgs(p_Function.GetNumArgs());
	}

	const std::size_t l_NumArgs = p_Command.GetArgs().size();
	const std::size_t l_NumRequiredArgs = p_Function.GetNumArgs();

	if (l_NumArgs == l_NumRequiredArgs)
	{
		try
		{
			p_Function._Call(p_Command.GetArgs());
		}
		catch (boost::bad_lexical_cast&)
		{
			std::cout << "Failed to convert arguments to function parameter types.\n\n";
		}
	}
	else
	{
		if (l_NumArgs > l_NumRequiredArgs)
		{
			std::cout << "Too many arguments passed: ";
		}
		else
		{
			std::cout << "Too few arguments passed: ";
		}

		std::cout << l_NumArgs << "/" << l_NumRequiredArgs << "\n";
		std::cout << "Usage: " << p_Function.GetName();

		if (!p_Function.GetArgsDesc().empty())
		{
			std::cout << " " << p_Function.GetArgsDesc();
		}
		std::cout << "\n\n";
	}
}
}
#endif
#include "pch.h"
#ifdef HAWK_DEBUG
#include "ConsoleCommandManager.h"
#include "ConsoleInputParser.h"
#include "ConsoleAPI.h"
#include "System/Version.h"
#include "Util/Algorithm.h"
#include "Util/StringUtil.h"
#include <iomanip>

namespace Hawk {

namespace 
{
	const char Key_Shift = 75;
	const char Key_Return = 13;
	const char Key_Backspace = 8;
	const char Key_Tab = 9;
}

ConsoleCommandManager::ConsoleCommandManager(std::shared_ptr<Dispatcher>& p_Dispatcher)
: m_Dispatcher(p_Dispatcher)
, m_bStopSignal(false)
, m_Mutex("ConsoleCommandManager")
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
	LOG("Console input thread started", "console", Info);
	Logger::RegisterThread("Thread_Console_Input", std::this_thread::get_id());

	std::string l_CurrLine;
	std::string l_CurrTypedLine;
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
							l_CurrTypedLine = l_CurrLine;
						}
						else if (l_cChr == Key_Tab)
						{
							ConsoleAPI::ClearCurrLine();
							l_CurrLine = GetNextCommand(l_CurrTypedLine, l_CurrLine);
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
								ConsoleInputParser l_ParsedInput(l_CurrLine);

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
							}
							l_CurrLine.clear();
							l_CurrTypedLine.clear();
						}
						else
						{
							l_CurrLine += l_cChr;
							l_CurrTypedLine = l_CurrLine;
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
	std::cout << Config::Instance().Get<std::string>(p_Key, "Config key not found") << "            \n\n";
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
		catch (boost::bad_lexical_cast& e)
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